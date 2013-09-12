#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/boost/graph/properties_Polyhedron_3.h>

#include <iostream>
#include <list>

#include <boost/graph/kruskal_min_spanning_tree.hpp>


typedef CGAL::Simple_cartesian<double>                       Kernel;
typedef Kernel::Vector_3                                     Vector;
typedef Kernel::Point_3                                      Point;
typedef CGAL::Polyhedron_3<Kernel>                           Polyhedron;

typedef boost::graph_traits<Polyhedron>::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits<Polyhedron>::vertex_iterator   vertex_iterator;
typedef boost::graph_traits<Polyhedron>::edge_descriptor   edge_descriptor;

// The BGL makes heavy use of indices associated to the vertices
// We use a std::map to store the index

typedef std::map<vertex_descriptor,int> VertexIndexMap;
VertexIndexMap vertex_id_map;

// A std::map is not a property map, because it is not lightweight
typedef boost::associative_property_map<VertexIndexMap> VertexIdPropertyMap;
VertexIdPropertyMap vertex_index_pmap(vertex_id_map);

void
kruskal(const Polyhedron& P)
{
  // associate indices to the vertices
  {
    vertex_iterator vb, ve;
    int index = 0;

    // boost::tie assigns the first and second element of the std::pair
    // returned by boost::vertices to the variables vit and ve
    for(boost::tie(vb,ve)=boost::vertices(P); vb!=ve; ++vb ){
      vertex_descriptor  vd = *vb;
      vertex_id_map[vd]= index++;
    }
  }
  
  // We use the default edge weight which is the squared length of the edge
  // This property map is defined in graph_traits_Polyhedron_3.h

  // In the function call you can see a named parameter: vertex_index_map
   std::list<edge_descriptor> mst;

   boost::kruskal_minimum_spanning_tree(P, 
					std::back_inserter(mst), 
					boost::vertex_index_map(vertex_index_pmap) );

   std::cout << "#VRML V2.0 utf8\n"
      "Shape {\n"
      "appearance Appearance {\n"
      "material Material { emissiveColor 1 0 0}}\n"
      "geometry\n"
      "IndexedLineSet {\n"
      "coord Coordinate {\n"
      "point [ \n";

   vertex_iterator vb, ve;
    for(boost::tie(vb,ve) = boost::vertices(P); vb!=ve; ++vb){
      std::cout << (*vb)->point() << "\n";
    }

    std::cout << "]\n"
      "}\n"
      "coordIndex [\n";

    for(std::list<edge_descriptor>::iterator it = mst.begin(); it != mst.end(); ++it)
    {
      edge_descriptor e = *it ;
      vertex_descriptor s = boost::source(e,P);
      vertex_descriptor t = boost::target(e,P);
      std::cout << vertex_id_map[s] << ", " << vertex_id_map[t] <<  ", -1\n";
    }

    std::cout << "]\n"
      "}#IndexedLineSet\n"
      "}# Shape\n";
}


int main() {

    Polyhedron P;
    Point a(1,0,0);
    Point b(0,1,0);
    Point c(0,0,1);
    Point d(0,0,0);

    P.make_tetrahedron(a,b,c,d);

    kruskal(P);

    return 0;
}
