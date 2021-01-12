#     ______     _____ _           ________
#    / ____/___ / ___/(_)___ ___  /  _/ __ |
#   / /   / __ \\__ \/ / __ `__ \ / // / / /
#  / /___/ /_/ /__/ / / / / / / // // /_/ /
#  \____/\____/____/_/_/ /_/ /_/___/\____/
#  Kratos CoSimulationApplication
#
#  License:         BSD License, see license.txt
#
#  Main authors:    Philipp Bucher (https://github.com/philbucher)
#

# tests for the python exposure of CoSimIO::ModelPart

# python imports
import unittest

import CoSimIO


class CoSimIO_ModelPart(unittest.TestCase):
    maxDiff = None # to display all the diff

    def test_node(self):
        coords = [1.0, -2.7, 9.44]
        node_id = 16

        def Check(node_to_check):
            self.assertEqual(node_to_check.Id(), node_id)
            self.assertAlmostEqual(node_to_check.X(), coords[0])
            self.assertAlmostEqual(node_to_check.Y(), coords[1])
            self.assertAlmostEqual(node_to_check.Z(), coords[2])

            for i in range(3):
                self.assertAlmostEqual(coords[i], node_to_check.Coordinates()[i], msg=str(i))

        with self.subTest("from_coords"):
            node = CoSimIO.Node(node_id, coords[0], coords[1], coords[2])
            Check(node)

        with self.subTest("from_coords_array"):
            node = CoSimIO.Node(node_id, coords)
            Check(node)


    def test_print_node(self):
        coords = [1.0, -2.7, 9.44]
        node_id = 16

        node = CoSimIO.Node(node_id, coords)

        exp_string = "CoSimIO-Node; Id: 16\n    Coordinates: [ 1 | -2.7 | 9.44 ]\n"
        self.assertMultiLineEqual(str(node), exp_string)

    def test_element_basics(self):
        elem_id = 33
        elem_type = 5

        node = CoSimIO.Node(1, 0,0,0)

        element = CoSimIO.Element(elem_id, elem_type, [node])

        self.assertEqual(element.Id(), elem_id)
        self.assertEqual(element.Type(), elem_type)
        self.assertEqual(element.NumberOfNodes(), 1)

    def test_element_nodes(self):
        elem_id = 33
        elem_type = 5

        node_ids = [2, 159, 61]

        dummy_coords = [0,0,0]

        node_1 = CoSimIO.Node(node_ids[0], dummy_coords)
        node_2 = CoSimIO.Node(node_ids[1], dummy_coords)
        node_3 = CoSimIO.Node(node_ids[2], dummy_coords)

        element = CoSimIO.Element(elem_id, elem_type, [node_1, node_2, node_3])

        self.assertEqual(element.Id(), elem_id)
        self.assertEqual(element.Type(), elem_type)
        self.assertEqual(element.NumberOfNodes(), 3)

        for i, node in enumerate(element.Nodes):
            self.assertEqual(node.Id(), node_ids[i], msg=str(i))

    def test_print_element(self):
        dummy_coords = [0,0,0]
        node_1 = CoSimIO.Node(1, dummy_coords)
        node_2 = CoSimIO.Node(22, dummy_coords)
        node_3 = CoSimIO.Node(321, dummy_coords)

        element = CoSimIO.Element(65, 5, [node_1, node_2, node_3])

        exp_string = "CoSimIO-Element; Id: 65\n    Number of Nodes: 3\n    Node Ids: 1, 22, 321\n"
        self.assertMultiLineEqual(str(element), exp_string)

    def test_model_part_basics(self):
        model_part = CoSimIO.ModelPart("for_test")

        self.assertEqual(model_part.Name(), "for_test")
        self.assertEqual(model_part.NumberOfNodes(), 0)
        self.assertEqual(model_part.NumberOfElements(), 0)

    def test_model_part_create_new_node(self):
        model_part = CoSimIO.ModelPart("for_test")

        node_id = 691
        node_coords = [1.0, -2.7, 9.44]
        new_node = model_part.CreateNewNode(node_id, node_coords[0], node_coords[1], node_coords[2])
        self.assertEqual(model_part.NumberOfNodes(), 1)

        self.assertEqual(new_node.Id(), node_id)
        for i in range(3):
            self.assertAlmostEqual(new_node.Coordinates()[i], node_coords[i], msg=str(i))

    def test_model_part_create_new_nodes(self):
        model_part = CoSimIO.ModelPart("for_test")

        node_id = 691
        node_coords = [1.0, -2.7, 9.44]
        new_node = model_part.CreateNewNode(node_id, node_coords[0], node_coords[1], node_coords[2])
        self.assertEqual(model_part.NumberOfNodes(), 1)

        model_part.CreateNewNode(node_id+1, node_coords[0], node_coords[1], node_coords[2])
        model_part.CreateNewNode(node_id+2, node_coords[0], node_coords[1], node_coords[2])
        self.assertEqual(model_part.NumberOfNodes(), 3)

        self.assertEqual(new_node.Id(), node_id)
        for i in range(3):
            self.assertAlmostEqual(new_node.Coordinates()[i], node_coords[i], msg=str(i))

        for i, node in enumerate(model_part.Nodes):
            self.assertEqual(node.Id(), node_id+i, msg=str(i))

    def test_model_part_create_new_node_twice(self):
        model_part = CoSimIO.ModelPart("for_test")

        model_part.CreateNewNode(1, 0,0,0)
        self.assertEqual(model_part.NumberOfNodes(), 1)

        with self.assertRaisesRegex(Exception, 'Error: The Node with Id 1 exists already!'):
            model_part.CreateNewNode(1, 0,0,0)

    def test_model_part_get_node(self):
        model_part = CoSimIO.ModelPart("for_test")

        node_id = 691
        node_coords = [1.0, -2.7, 9.44]
        new_node = model_part.CreateNewNode(node_id, node_coords[0], node_coords[1], node_coords[2])
        self.assertEqual(model_part.NumberOfNodes(), 1)

        with self.subTest("existing"):
            node = model_part.GetNode(node_id)

            self.assertEqual(new_node.Id(), node_id)
            for i in range(3):
                self.assertAlmostEqual(new_node.Coordinates()[i], node_coords[i], msg=str(i))

        with self.subTest("non_existing"):
            with self.assertRaisesRegex(Exception, 'Error: Node with Id 692 does not exist!'):
                model_part.GetNode(node_id+1)

    def test_model_part_create_new_element(self):
        model_part = CoSimIO.ModelPart("for_test")

        node_id = 691
        node_coords = [1.0, -2.7, 9.44]
        new_node = model_part.CreateNewNode(node_id, node_coords[0], node_coords[1], node_coords[2])
        self.assertEqual(model_part.NumberOfNodes(), 1)

        elem_id = 47
        elem_type = 5

        new_elem = model_part.CreateNewElement(elem_id, elem_type, [node_id])

        self.assertEqual(new_elem.Id(), elem_id)
        self.assertEqual(new_elem.Type(), elem_type)
        self.assertEqual(new_elem.NumberOfNodes(), 1)

        for node in new_elem.Nodes:
            self.assertEqual(node.Id(), node_id)
            for i in range(3):
                self.assertAlmostEqual(node.Coordinates()[i], node_coords[i], msg=str(i))

    def test_model_part_create_new_elements(self):
        model_part = CoSimIO.ModelPart("for_test")

        node_ids = [2, 159, 61]
        node_coords = [1.0, -2.7, 9.44]
        model_part.CreateNewNode(node_ids[0], node_coords[0], node_coords[1], node_coords[2])
        model_part.CreateNewNode(node_ids[1], node_coords[1], node_coords[2], node_coords[0])
        model_part.CreateNewNode(node_ids[2], node_coords[2], node_coords[0], node_coords[1])

        self.assertEqual(model_part.NumberOfNodes(), 3)

        elem_ids = [21, 19, 961]
        elem_types = [5, 5, 9]
        elem_num_nodes = [1,1,2]

        model_part.CreateNewElement(elem_ids[0], elem_types[0], [node_ids[0]])
        model_part.CreateNewElement(elem_ids[1], elem_types[1], [node_ids[1]])
        model_part.CreateNewElement(elem_ids[2], elem_types[2], [node_ids[1], node_ids[2]])

        self.assertEqual(model_part.NumberOfElements(), 3)

        for i, elem in enumerate(model_part.Elements):
            self.assertEqual(elem.Id(), elem_ids[i])
            self.assertEqual(elem.Type(), elem_types[i])
            self.assertEqual(elem.NumberOfNodes(), elem_num_nodes[i])

    def test_model_part_create_new_element_twice(self):
        model_part = CoSimIO.ModelPart("for_test")

        model_part.CreateNewNode(1, 0,0,0)
        model_part.CreateNewElement(1, 5, [1])
        self.assertEqual(model_part.NumberOfElements(), 1)

        with self.assertRaisesRegex(Exception, 'Error: The Element with Id 1 exists already!'):
            model_part.CreateNewElement(1, 5, [1])

    def test_model_part_get_element(self):
        model_part = CoSimIO.ModelPart("for_test")

        model_part.CreateNewNode(1, 0,0,0)

        elem_id = 6
        model_part.CreateNewElement(elem_id, 5, [1])
        self.assertEqual(model_part.NumberOfElements(), 1)

        with self.subTest("existing"):
            elem = model_part.GetElement(elem_id)
            self.assertEqual(elem.Id(), elem_id)
            self.assertEqual(elem.Type(), 5)
            self.assertEqual(elem.NumberOfNodes(), 1)

        with self.subTest("non_existing"):
            with self.assertRaisesRegex(Exception, 'Error: Element with Id 7 does not exist!'):
                model_part.GetElement(elem_id+1)

    def test_print_model_part(self):
        model_part = CoSimIO.ModelPart("for_test")

        with self.subTest("empty"):
            exp_string = "CoSimIO-ModelPart \"for_test\"\n    Number of Nodes: 0\n    Number of Elements: 0\n"
            self.assertMultiLineEqual(str(model_part), exp_string)

        with self.subTest("with_entities"):
            node_ids = [2, 159, 61]
            node_coords = [1.0, -2.7, 9.44]
            model_part.CreateNewNode(node_ids[0], node_coords[0], node_coords[1], node_coords[2])
            model_part.CreateNewNode(node_ids[1], node_coords[1], node_coords[2], node_coords[0])
            model_part.CreateNewNode(node_ids[2], node_coords[2], node_coords[0], node_coords[1])

            model_part.CreateNewElement(15, 1, [node_ids[0]])

            exp_string = "CoSimIO-ModelPart \"for_test\"\n    Number of Nodes: 3\n    Number of Elements: 1\n"
            self.assertMultiLineEqual(str(model_part), exp_string)


if __name__ == '__main__':
    unittest.main()