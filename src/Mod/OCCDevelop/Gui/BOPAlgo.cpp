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

static TopoDS_Compound buildCompound(const OCCDevelop::ShapeVector& shapesIn)
{
  BRep_Builder builder;
  TopoDS_Compound out;
  builder.MakeCompound(out);
  for (const auto &currentShape : shapesIn)
    builder.Add(out, currentShape);
  return out;
}

void OCCDevelop::BOPAlgo(const OCCDevelop::ShapeVector &ssIn)
{
  //when splitting edges we will have to compare the number of output edges to
  //the input edges too see if the operation did anything.
    
  //images is a map from input to output.
  //origins is a map from output to input.
  
  try
  {
    BOPAlgo_Builder bopBuilder;
    for (const auto &s : ssIn)
      bopBuilder.AddArgument(s);

    bopBuilder.Perform();
    if (bopBuilder.HasErrors())
    {
      std::ostringstream error;
      error << "error with bopBuilder. error code: "
      << std::endl;
      bopBuilder.DumpErrors(error);
      throw std::runtime_error(error.str());
    }
    
    const BOPCol_DataMapOfShapeListOfShape &images = bopBuilder.Images();
    BOPCol_DataMapOfShapeListOfShape::Iterator imageIt(images);
    for (int index = 0; imageIt.More(); imageIt.Next(), index++)
    {
      const TopoDS_Shape &key = imageIt.Key();
      std::ostringstream keyBase;
      keyBase << "Output_Images_key_" << ((index < 10) ? "0" : "") << index;
      std::ostringstream keyName;
      keyName << keyBase.str() << "_source_shapetype:" << shapeText(key.ShapeType());
      addFeature(keyName.str(), key);
      
      int count = 0;
      for (const auto &sil : imageIt.Value()) //shape in list
      {
        count++;
        assert(key.ShapeType() == sil.ShapeType());
        
        std::ostringstream mappedName;
        mappedName << keyBase.str() << "_result_shape_" << ((count < 10) ? "0" : "") << count;
        
        addFeature(mappedName.str(), sil);
      }
    }
    
    //it appears splits only contain faces that have been split. It ignores edges.
    const BOPCol_DataMapOfShapeListOfShape &splits = bopBuilder.Splits();
    BOPCol_DataMapOfShapeListOfShape::Iterator splitIt(splits);
    for (int index = 0; splitIt.More(); splitIt.Next(), index++)
    {
      const TopoDS_Shape &key = splitIt.Key();
      std::ostringstream keyBase;
      keyBase << "Output_Splits_key_" << ((index < 10) ? "0" : "") << index;
      std::ostringstream keyName;
      keyName << keyBase.str() << "_source_shapetype:" << shapeText(key.ShapeType());
      addFeature(keyName.str(), key);
      
      int count = 0;
      for (const auto &sil : splitIt.Value()) //shape in list
      {
        count++;
        assert(key.ShapeType() == sil.ShapeType());
        
        std::ostringstream mappedName;
        mappedName << keyBase.str() << "_result_shape_" << ((count < 10) ? "0" : "") << count;
        
        addFeature(mappedName.str(), sil);
      }
    }
    
    //origins. the keys are the new shapes and the mapped are the originals.
    //allows to trace resultant geometry to source geometry. ignores at least compounds and wires.
    const BOPCol_DataMapOfShapeListOfShape &origins = bopBuilder.Origins();
    BOPCol_DataMapOfShapeListOfShape::Iterator originIt(origins);
    for (int index = 0; originIt.More(); originIt.Next(), index++)
    {
      const TopoDS_Shape &key = originIt.Key();
      std::ostringstream keyBase;
      keyBase << "Output_Origins_key_" << ((index < 10) ? "0" : "") << index;
      std::ostringstream keyName;
      keyName << keyBase.str() << "_source_shapetype:" << shapeText(key.ShapeType());
      addFeature(keyName.str(), key);
      int mappedCount = 0;
      for (const auto &sil : origins(key)) //shape in list
      {
        assert(key.ShapeType() == sil.ShapeType());
        
        std::ostringstream mappedName;
        mappedName << keyBase.str() << "_result_shape_" << ((mappedCount < 10) ? "0" : "") << mappedCount;
        
        addFeature(mappedName.str(), sil);
        mappedCount++;
      }
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
    
    int count = 0;
    //Generated for inputs. This was empty for the 2 face intersect.
    //I didn't update this as it produces nothing at this time occt 7.2
    for (const auto &s : ssIn)
    {
      count++;
      TopTools_IndexedMapOfShape inputShapeMap;
      TopExp::MapShapes(s, inputShapeMap);
      for (int index = 1; index <= inputShapeMap.Extent(); ++index)
      {
        const TopoDS_Shape &key = inputShapeMap.FindKey(index);
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
        keyBase << "Output_Generated_base" << count << "_Shape" << ((index < 10) ? "0" : "") << index;
        std::ostringstream keyName;
        keyName << keyBase.str() << "_source_shapetype:" << shapeText(key.ShapeType());
        
        std::ostringstream mappedName;
        mappedName << keyBase.str() << "_result_count_" << mappedVector.size();
        
        addFeature(keyName.str(), key);
        addFeature(mappedName.str(), buildCompound(mappedVector));
      }
    }
    
    //Modified for input 1. ignores at least compounds, shells and wires.
    count = 0;
    for (const auto &s : ssIn)
    {
      count++;
      TopTools_IndexedMapOfShape inputShapeMap;
      TopExp::MapShapes(s, inputShapeMap);
      for (int index = 1; index <= inputShapeMap.Extent(); ++index)
      {
        const TopoDS_Shape &key = inputShapeMap.FindKey(index);
        const TopTools_ListOfShape &modified = bopBuilder.Modified(key);
        if (modified.IsEmpty())
          continue;
        std::ostringstream keyBase;
        keyBase << "Output_Modified_base" << count << "_Shape" << ((index < 10) ? "0" : "") << index;
        std::ostringstream keyName;
        keyName << keyBase.str() << "_source_shapetype:" << shapeText(key.ShapeType());
        addFeature(keyName.str(), key);
        
        
        
        int count = 0;
        for (const auto &sil : modified) //shape in list
        {
          count++;
          assert(key.ShapeType() == sil.ShapeType());
          
          std::ostringstream mappedName;
          mappedName << keyBase.str() << "_result_shape_" << ((count < 10) ? "0" : "") << count;
          
          addFeature(mappedName.str(), sil);
        }
      }
    }
    
    //IsDeleted for inputs.
    count = 0;
    for (const auto &s : ssIn)
    {
      count++;
      TopTools_IndexedMapOfShape inputShapeMap;
      TopExp::MapShapes(s, inputShapeMap);
      for (int index = 1; index <= inputShapeMap.Extent(); ++index)
      {
        const TopoDS_Shape &key = inputShapeMap.FindKey(index);
        if (!bopBuilder.IsDeleted(key))
          continue;
        
        std::ostringstream keyBase;
        keyBase << "Input_Deleted_base" << count << "_Shape" << ((index < 10) ? "0" : "") << index;
        keyBase << "_shapetype:" << shapeText(key.ShapeType());
        
        addFeature(keyBase.str(), key);
      }
    }
  }
  catch (const Standard_Failure &e)
  {
    std::cout << "OCC Error: " << e.GetMessageString() << std::endl;
  }
  catch (const std::exception &error)
  {
    std::cout << "My Error: " << error.what() << std::endl;
  }
}
