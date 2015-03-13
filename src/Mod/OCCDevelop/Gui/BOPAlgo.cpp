/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2015  tanderson <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdexcept>
#include <vector>

#include "PreCompiled.h"
#ifndef _PreComp_
  #include <Standard_StdAllocator.hxx>
  #include <TopoDS_Shape.hxx>
  #include <BRepBuilderAPI_Copy.hxx>
  #include <Standard_Failure.hxx>
  #include <BOPAlgo_Builder.hxx>
  #include <BRep_Builder.hxx>
  #include <TopExp.hxx>
  #include <TopTools_IndexedMapOfShape.hxx>
  #include <TopTools_ListIteratorOfListOfShape.hxx>
#endif

#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Mod/Part/App/PartFeature.h>

#include "BOPAlgo.h"

typedef std::vector<TopoDS_Shape, Standard_StdAllocator<TopoDS_Shape> > ShapeVector;

static std::string shapeText(TopAbs_ShapeEnum shapeType)
{
    static std::vector<std::string> names
    ({
      "Compound",
      "CompSolid",
      "Solid",
      "Shell",
      "Face",
      "Wire",
      "Edge",
      "Vertex",
      "Shape"
    });
    
    return names.at(static_cast<int>(shapeType));
}

static void addFeature(const std::string &nameIn, const TopoDS_Shape &shapeIn)
{
  Part::Feature *feature = dynamic_cast<Part::Feature *>(
    App::GetApplication().getActiveDocument()->addObject("Part::Feature", nameIn.c_str()));
  assert(feature);
  feature->Shape.setValue(shapeIn);
}

static TopoDS_Compound buildCompound(const ShapeVector& shapesIn)
{
  BRep_Builder builder;
  TopoDS_Compound out;
  builder.MakeCompound(out);
  for (const auto &currentShape : shapesIn)
    builder.Add(out, currentShape);
  return out;
}

void OCCDevelop::BOPAlgo(const TopoDS_Shape& shape1In, const TopoDS_Shape& shape2In)
{
  //when splitting edges we will have to compare the number of output edges to
  //the input edges too see if the operation did anything.
    
  //images is a map from input to output.
  //origins is a map from output to input.
  
  try
  {
    BRepBuilderAPI_Copy copier;
    copier.Perform(shape1In);
    TopoDS_Shape copy1 = copier.Shape();
    copier.Perform(shape2In);
    TopoDS_Shape copy2 = copier.Shape();
    
    BOPAlgo_Builder bopBuilder;
    bopBuilder.AddArgument(copy1);
    bopBuilder.AddArgument(copy2);
    bopBuilder.Perform();
    if (bopBuilder.ErrorStatus())
    {
      std::ostringstream error;
      error << "error with bopBuilder. error code: " << bopBuilder.ErrorStatus() << std::endl;
      throw std::runtime_error(error.str());
    }
    
    const BOPCol_DataMapOfShapeListOfShape &images = bopBuilder.Images();
    BOPCol_DataMapOfShapeListOfShape::Iterator imageIt(images);
    for (int index = 0; imageIt.More(); imageIt.Next(), index++)
    {
      const TopoDS_Shape &key = imageIt.Key();
      
      const BOPCol_ListOfShape &shapeList = imageIt.Value();
      BOPCol_ListOfShape::Iterator shapeListIt(shapeList);
      ShapeVector mappedVector;
      for (;shapeListIt.More();shapeListIt.Next())
        mappedVector.push_back(shapeListIt.Value());
      //I have verified that shapetype of key equals all shapetype of all the objects in shapeList.
      //meaning if the key is of type face, then shapelist only contains faces.
      
      std::ostringstream keyBase;
      keyBase << "Output_Images_key_" << ((index < 10) ? "0" : "") << index;
      std::ostringstream keyName;
      keyName << keyBase.str() << "_source_shapetype:" << shapeText(key.ShapeType());
      
      std::ostringstream mappedName;
      mappedName << keyBase.str() << "_result_count_" << mappedVector.size();
      
      addFeature(keyName.str(), key);
      addFeature(mappedName.str(), buildCompound(mappedVector));
    }
    
    //it appears splits only contain faces that have been split. It ignores edges.
    const BOPCol_DataMapOfShapeListOfShape &splits = bopBuilder.Splits();
    BOPCol_DataMapOfShapeListOfShape::Iterator splitIt(splits);
    for (int index = 0; splitIt.More(); splitIt.Next(), index++)
    {
      const TopoDS_Shape &key = splitIt.Key();
      
      const BOPCol_ListOfShape &shapeList = splitIt.Value();
      BOPCol_ListOfShape::Iterator shapeListIt(shapeList);
      ShapeVector mappedVector;
      for (;shapeListIt.More();shapeListIt.Next())
        mappedVector.push_back(shapeListIt.Value());
      //I have verified that shapetype of key equals all shapetype of all the objects in shapeList.
      //meaning if the key is of type face, then shapelist only contains faces.
      
      std::ostringstream keyBase;
      keyBase << "Output_Splits_key_" << ((index < 10) ? "0" : "") << index;
      std::ostringstream keyName;
      keyName << keyBase.str() << "_source_shapetype:" << shapeText(key.ShapeType());
      
      std::ostringstream mappedName;
      mappedName << keyBase.str() << "_result_count_" << mappedVector.size();
      
      addFeature(keyName.str(), key);
      addFeature(mappedName.str(), buildCompound(mappedVector));
    }
    
    //origins. the keys are the new shapes and the mapped are the originals.
    //allows to trace resultant geometry to source geometry. ignores at least compounds and wires.
    const BOPCol_DataMapOfShapeShape &origins = bopBuilder.Origins();
    BOPCol_DataMapOfShapeShape::Iterator originIt(origins);
    for (int index = 0; originIt.More(); originIt.Next(), index++)
    {
      const TopoDS_Shape &key = originIt.Key();
      const TopoDS_Shape &mapped = origins(key);
      assert(key.ShapeType() == mapped.ShapeType());
      
      std::ostringstream keyBase;
      keyBase << "Output_Origins_key_" << ((index < 10) ? "0" : "") << index;
      std::ostringstream keyName;
      keyName << keyBase.str() << "_source_shapetype:" << shapeText(key.ShapeType());
      std::ostringstream mappedName;
      mappedName << keyBase.str() << "_result_shape";
      
      addFeature(keyName.str(),key);
      addFeature(mappedName.str(), mapped);
    }
    
    //shapesSD. This is empty for the 2 face intersect.
    const BOPCol_DataMapOfShapeShape &shapesSD = bopBuilder.ShapesSD();
    BOPCol_DataMapOfShapeShape::Iterator shapesSDIt(shapesSD);
    for (int index = 0; shapesSDIt.More(); shapesSDIt.Next(), index++)
    {
      const TopoDS_Shape &key = shapesSDIt.Key();
      const TopoDS_Shape &mapped = shapesSD(key);
      assert(key.ShapeType() == mapped.ShapeType());
      
      std::ostringstream keyBase;
      keyBase << "Output_ShapesSD_key_" << ((index < 10) ? "0" : "") << index;
      std::ostringstream keyName;
      keyName << keyBase.str() << "_source_shapetype:" << shapeText(key.ShapeType());
      std::ostringstream mappedName;
      mappedName << keyBase.str() << "_result_shape";
      
      addFeature(keyName.str(), key);
      addFeature(mappedName.str(), mapped);
    }
    
    //Generated for input 1. This is empty for the 2 face intersect.
    TopTools_IndexedMapOfShape inputShape1Map, inputShape2Map;
    TopExp::MapShapes(copy1, inputShape1Map);
    TopExp::MapShapes(copy2, inputShape2Map);
    for (int index = 1; index <= inputShape1Map.Extent(); ++index)
    {
      const TopoDS_Shape &key = inputShape1Map.FindKey(index);
      const TopTools_ListOfShape &generated = bopBuilder.Generated(key);
      if (generated.IsEmpty())
        continue;
      
      ShapeVector mappedVector;
      TopTools_ListIteratorOfListOfShape listIt(generated);
      for (;listIt.More();listIt.Next())
      {
        mappedVector.push_back(listIt.Value());
        assert(listIt.Value().ShapeType() == key.ShapeType());
      }
      
      std::ostringstream keyBase;
      keyBase << "Output_Generated_base1_" << ((index < 10) ? "0" : "") << index;
      std::ostringstream keyName;
      keyName << keyBase.str() << "_source_shapetype:" << shapeText(key.ShapeType());
      
      std::ostringstream mappedName;
      mappedName << keyBase.str() << "_result_count_" << mappedVector.size();
      
      addFeature(keyName.str(), key);
      addFeature(mappedName.str(), buildCompound(mappedVector));
    }
    
    //Generated for input 2. This is empty for the 2 face intersect.
    for (int index = 1; index <= inputShape2Map.Extent(); ++index)
    {
      const TopoDS_Shape &key = inputShape2Map.FindKey(index);
      const TopTools_ListOfShape &generated = bopBuilder.Generated(key);
      if (generated.IsEmpty())
        continue;
      
      ShapeVector mappedVector;
      TopTools_ListIteratorOfListOfShape listIt(generated);
      for (;listIt.More();listIt.Next())
      {
        mappedVector.push_back(listIt.Value());
        assert(listIt.Value().ShapeType() == key.ShapeType());
      }
      
      std::ostringstream keyBase;
      keyBase << "Output_Generated_base2_" << ((index < 10) ? "0" : "") << index;
      std::ostringstream keyName;
      keyName << keyBase.str() << "_source_shapetype:" << shapeText(key.ShapeType());
      
      std::ostringstream mappedName;
      mappedName << keyBase.str() << "_result_count_" << mappedVector.size();
      
      addFeature(keyName.str(), key);
      addFeature(mappedName.str(), buildCompound(mappedVector));
    }
    
    //Modified for input 1. ignores at least compounds, shells and wires.
    for (int index = 1; index <= inputShape1Map.Extent(); ++index)
    {
      const TopoDS_Shape &key = inputShape1Map.FindKey(index);
      const TopTools_ListOfShape &generated = bopBuilder.Modified(key);
      if (generated.IsEmpty())
        continue;
      
      ShapeVector mappedVector;
      TopTools_ListIteratorOfListOfShape listIt(generated);
      for (;listIt.More();listIt.Next())
      {
        mappedVector.push_back(listIt.Value());
        assert(listIt.Value().ShapeType() == key.ShapeType());
      }
      
      std::ostringstream keyBase;
      keyBase << "Output_Modified_base1_" << ((index < 10) ? "0" : "") << index;
      std::ostringstream keyName;
      keyName << keyBase.str() << "_source_shapetype:" << shapeText(key.ShapeType());
      
      std::ostringstream mappedName;
      mappedName << keyBase.str() << "_result_count_" << mappedVector.size();
      
      addFeature(keyName.str(), key);
      addFeature(mappedName.str(), buildCompound(mappedVector));
    }
    
    //Modified for input 2. ignores at least compounds, shells and wires.
    for (int index = 1; index <= inputShape2Map.Extent(); ++index)
    {
      const TopoDS_Shape &key = inputShape2Map.FindKey(index);
      const TopTools_ListOfShape &generated = bopBuilder.Modified(key);
      if (generated.IsEmpty())
        continue;
      
      ShapeVector mappedVector;
      TopTools_ListIteratorOfListOfShape listIt(generated);
      for (;listIt.More();listIt.Next())
      {
        mappedVector.push_back(listIt.Value());
        assert(listIt.Value().ShapeType() == key.ShapeType());
      }
      
      std::ostringstream keyBase;
      keyBase << "Output_Modified_base2_" << ((index < 10) ? "0" : "") << index;
      std::ostringstream keyName;
      keyName << keyBase.str() << "_source_shapetype:" << shapeText(key.ShapeType());
      
      std::ostringstream mappedName;
      mappedName << keyBase.str() << "_result_count_" << mappedVector.size();
      
      addFeature(keyName.str(), key);
      addFeature(mappedName.str(), buildCompound(mappedVector));
    }
  }
  catch (Standard_Failure)
  {
    Handle_Standard_Failure e = Standard_Failure::Caught();
    std::cout << "OCC Error: " << e->GetMessageString() << std::endl;
  }
  catch (const std::exception &error)
  {
    std::cout << "My Error: " << error.what() << std::endl;
  }
}
