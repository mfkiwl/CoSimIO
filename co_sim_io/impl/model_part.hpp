//     ______     _____ _           ________
//    / ____/___ / ___/(_)___ ___  /  _/ __ |
//   / /   / __ \\__ \/ / __ `__ \ / // / / /
//  / /___/ /_/ /__/ / / / / / / // // /_/ /
//  \____/\____/____/_/_/ /_/ /_/___/\____/
//  Kratos CoSimulationApplication
//
//  License:         BSD License, see license.txt
//
//  Main authors:    Philipp Bucher (https://github.com/philbucher)
//

#ifndef CO_SIM_IO_MODEL_PART_H_INCLUDED
#define CO_SIM_IO_MODEL_PART_H_INCLUDED

/* This file contains the implementation of th  CoSimIO::ModelPart
It serves as a data container when exchanging data
Also it is used in order to be consistent with Kratos to reduce compatibility problems
This is a simplified version of Kratos::ModelPart
see https://github.com/KratosMultiphysics/Kratos/blob/master/kratos/includes/model_part.h
*/

// System includes
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>

// Project includes
#include "define.hpp"
#include "macros.hpp"

namespace CoSimIO {
class Node
{
public:
    Node(
        const IdType I_Id,
        const double I_X,
        const double I_Y,
        const double I_Z)
    : mId(I_Id),
      mX(I_X),
      mY(I_Y),
      mZ(I_Z)
    {
        CO_SIM_IO_ERROR_IF(I_Id < 1) << "Id must be >= 1!" << std::endl;
    }

    Node(
        const IdType I_Id,
        const CoordinatesType& I_Coordinates)
    : Node(I_Id, I_Coordinates[0], I_Coordinates[1], I_Coordinates[2])
    { }

    // delete copy and assignment CTor
    Node(const Node&) = delete;
    Node& operator=(Node const&) = delete;

    IdType Id() const { return mId; }
    double X() const { return mX; }
    double Y() const { return mY; }
    double Z() const { return mZ; }
    CoordinatesType Coordinates() const { return {mX, mY, mZ}; }

private:
    IdType mId;
    double mX;
    double mY;
    double mZ;
};

class Element
{
public:
    using ElementType = std::size_t;
    using NodesContainerType = std::vector<Node*>;
    using ConnectivitiesType = std::vector<IdType>;

    Element(
        const IdType I_Id,
        const ElementType I_Type,
        const NodesContainerType& I_Nodes)
    : mId(I_Id),
      mType(I_Type),
      mNodes(I_Nodes)
    {
        CO_SIM_IO_ERROR_IF(I_Id < 1) << "Id must be >= 1!" << std::endl;
        CO_SIM_IO_ERROR_IF(NumberOfNodes() < 1) << "No nodes were passed!" << std::endl;
    }

    // delete copy and assignment CTor
    Element(const Element&) = delete;
    Element& operator=(Element const&) = delete;

    IdType Id() const { return mId; }
    ElementType Type() const { return mType; }
    std::size_t NumberOfNodes() const { return mNodes.size(); }
    NodesContainerType::const_iterator NodesBegin() const { return mNodes.begin(); }
    NodesContainerType::const_iterator NodesEnd() const { return mNodes.end(); }

private:
    IdType mId;
    ElementType mType;
    NodesContainerType mNodes;
};

class ModelPart
{
public:

    using NodePointerType = std::shared_ptr<Node>; // TODO switch to intrusive_ptr
    using ElementPointerType = std::shared_ptr<Element>; // TODO switch to intrusive_ptr
    using NodesContainerType = std::vector<NodePointerType>;
    using ElementsContainerType = std::vector<ElementPointerType>;

    explicit ModelPart(const std::string& I_Name) : mName(I_Name)
    {
        CO_SIM_IO_ERROR_IF(I_Name.empty()) << "Please don't use empty names (\"\") when creating a ModelPart" << std::endl;
        CO_SIM_IO_ERROR_IF_NOT(I_Name.find(".") == std::string::npos) << "Please don't use names containing (\".\") when creating a ModelPart (used in \"" << I_Name << "\")" << std::endl;
    }

    // delete copy and assignment CTor
    ModelPart(const ModelPart&) = delete;
    ModelPart& operator=(ModelPart const&) = delete;

    std::string Name() const { return mName; }
    std::size_t NumberOfNodes() const { return mNodes.size(); }
    std::size_t NumberOfElements() const { return mElements.size(); }

    Node& CreateNewNode(
        const IdType I_Id,
        const double I_X,
        const double I_Y,
        const double I_Z)
    {
        CO_SIM_IO_ERROR_IF(HasNode(I_Id)) << "The Node with Id " << I_Id << " exists already!" << std::endl;

        mNodes.push_back(std::make_shared<Node>(I_Id, I_X, I_Y, I_Z));
        return *(mNodes.back());
    }

    Element& CreateNewElement(
        const IdType I_Id,
        const Element::ElementType I_Type,
        const Element::ConnectivitiesType& I_Connectivities)
    {
        CO_SIM_IO_ERROR_IF(HasElement(I_Id)) << "The Element with Id " << I_Id << " exists already!" << std::endl;

        Element::NodesContainerType nodes;
        nodes.reserve(I_Connectivities.size());
        for (const IdType node_id : I_Connectivities) {
            nodes.push_back(&GetNode(node_id));
        }
        mElements.push_back(std::make_shared<Element>(I_Id, I_Type, nodes));
        return *(mElements.back());
    }

    NodesContainerType::const_iterator NodesBegin() const { return mNodes.begin(); }
    ElementsContainerType::const_iterator ElementsBegin() const { return mElements.begin(); }

    NodesContainerType::const_iterator NodesEnd() const { return mNodes.end(); }
    ElementsContainerType::const_iterator ElementsEnd() const { return mElements.end(); }

    Node& GetNode(const IdType I_Id)
    {
        auto it_node = FindNode(I_Id);
        CO_SIM_IO_ERROR_IF(it_node == mNodes.end()) << "Node with Id " << I_Id << " does not exist!" << std::endl;
        return **it_node;
    }

    const Node& GetNode(const IdType I_Id) const
    {
        auto it_node = FindNode(I_Id);
        CO_SIM_IO_ERROR_IF(it_node == mNodes.end()) << "Node with Id " << I_Id << " does not exist!" << std::endl;
        return **it_node;
    }

    Element& GetElement(const IdType I_Id)
    {
        auto it_elem = FindElement(I_Id);
        CO_SIM_IO_ERROR_IF(it_elem == mElements.end()) << "Element with Id " << I_Id << " does not exist!" << std::endl;
        return **it_elem;
    }

    const Element& GetElement(const IdType I_Id) const
    {
        auto it_elem = FindElement(I_Id);
        CO_SIM_IO_ERROR_IF(it_elem == mElements.end()) << "Element with Id " << I_Id << " does not exist!" << std::endl;
        return **it_elem;
    }

private:
    std::string mName;
    NodesContainerType mNodes;
    ElementsContainerType mElements;

    NodesContainerType::const_iterator FindNode(const IdType I_Id) const
    {
        return std::find_if(
            mNodes.begin(), mNodes.end(),
            [I_Id](const NodePointerType& rp_node) { return rp_node->Id() == I_Id;});
    }

    NodesContainerType::iterator FindNode(const IdType I_Id)
    {
        return std::find_if(
            mNodes.begin(), mNodes.end(),
            [I_Id](const NodePointerType& rp_node) { return rp_node->Id() == I_Id;});
    }

    ElementsContainerType::const_iterator FindElement(const IdType I_Id) const
    {
        return std::find_if(
            mElements.begin(), mElements.end(),
            [I_Id](const ElementPointerType& rp_elem) { return rp_elem->Id() == I_Id;});
    }

    ElementsContainerType::iterator FindElement(const IdType I_Id)
    {
        return std::find_if(
            mElements.begin(), mElements.end(),
            [I_Id](const ElementPointerType& rp_elem) { return rp_elem->Id() == I_Id;});
    }

    bool HasNode(const IdType I_Id) const
    {
        return FindNode(I_Id) != mNodes.end();
    }

    bool HasElement(const IdType I_Id) const
    {
        return FindElement(I_Id) != mElements.end();
    }
};

} //namespace CoSimIO

#endif // CO_SIM_IO_MODEL_PART_H_INCLUDED