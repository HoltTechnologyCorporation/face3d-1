#include "semantic_map.h"
#include <algorithm>
#include <string>
#include <cmath>
#include <vgl/vgl_point_3d.h>


const vgl_box_3d<double> face3d::default_semantic_map_bbox = vgl_box_3d<double>(-100.0, -155.0, -100.0,
                                                                                100.0, 130.0, 120.0);

const vgl_box_3d<double> face3d::flame_semantic_map_bbox = vgl_box_3d<double>(-0.100, -0.155, -0.100,
                                                                                0.100, 0.130, 0.120);

const vgl_box_3d<double> face3d::noneck_semantic_map_bbox = vgl_box_3d<double>(-100.0, -130.0, -120.0,
                                                                                100.0, 130.0, 120.0);

const vgl_box_3d<double> face3d::MM_semantic_map_bbox = vgl_box_3d<double>(-100.0, -130.0, -25.0,
                                                                           100.0, 100.0, 150.0);

const vgl_box_3d<double> face3d::default_img3d_bbox = vgl_box_3d<double>(-100.0, -155.0, -100.0,
                                                                         100.0, 130.0, 120.0);

const vgl_box_3d<double> face3d::flame_img3d_bbox = vgl_box_3d<double>(-0.100, -0.155, -0.100,
                                                                         0.100, 0.130, 0.120);

const vgl_box_3d<double> face3d::noneck_img3d_bbox = vgl_box_3d<double>(-100.0, -130.0, -120.0,
                                                                        100.0, 130.0, 120.0);

const vgl_box_3d<double> face3d::MM_img3d_bbox = vgl_box_3d<double>(-100.0, -130.0, -25.0,
                                                                    100.0, 100.0, 150.0);

const vgl_box_3d<double> face3d::default_offsets_bbox = vgl_box_3d<double>(-20.0, -20.0, -20.0,
                                                                            20.0,  20.0,  20.0);
const vgl_box_3d<double> face3d::flame_offsets_bbox = vgl_box_3d<double>(-0.020, -0.02, -0.02,
                                                                            0.02,  0.02,  0.02);

const vgl_box_3d<double> face3d::MM_offsets_bbox = vgl_box_3d<double>(-25.0, -25.0, -25.0,
                                                                      25.0,  25.0,  25.0);

void face3d::normalize_3d_image(vil_image_view<float> const& in, vgl_box_3d<double> const& bbox,
                                vil_image_view<unsigned char> &out)
{
  const int nx = in.ni();
  const int ny = in.nj();
  out.set_size(nx,ny,3);
  out.fill(0);

  float min_x = bbox.min_x();
  float x_range = bbox.max_x() - bbox.min_x();
  float min_y = bbox.min_y();
  float y_range = bbox.max_y() - bbox.min_y();
  float min_z = bbox.min_z();
  float z_range = bbox.max_z() - bbox.min_z();

  for (unsigned int yi=0; yi<ny; ++yi) {
    for (unsigned int xi=0; xi<nx; ++xi) {
      vgl_point_3d<float> source_pt(in(xi,yi,0), in(xi,yi,1), in(xi,yi,2));
      if (std::isnan(source_pt.x()) || std::isnan(source_pt.y()) || std::isnan(source_pt.z())) {
        continue;
      }
      out(xi,yi,0) = static_cast<unsigned char>(std::max(0.0f,std::min(255.0f,(source_pt.x() - min_x)/x_range * 255)));
      out(xi,yi,1) = static_cast<unsigned char>(std::max(0.0f,std::min(255.0f,(source_pt.y() - min_y)/y_range * 255)));
      out(xi,yi,2) = static_cast<unsigned char>(std::max(0.0f,std::min(255.0f,(source_pt.z() - min_z)/z_range * 255)));
    }
  }
  return;
}

void face3d::unnormalize_3d_image(vil_image_view<unsigned char> const& in, vgl_box_3d<double> const& bbox,
                                  vil_image_view<float> &out)
{
  const int nx = in.ni();
  const int ny = in.nj();
  out.set_size(nx,ny,3);

  float min_x = bbox.min_x();
  float x_range = bbox.max_x() - bbox.min_x();
  float min_y = bbox.min_y();
  float y_range = bbox.max_y() - bbox.min_y();
  float min_z = bbox.min_z();
  float z_range = bbox.max_z() - bbox.min_z();

  for (unsigned int yi=0; yi<ny; ++yi) {
    for (unsigned int xi=0; xi<nx; ++xi) {
      float x = static_cast<float>(in(xi,yi,0))/255 * x_range + min_x;
      float y = static_cast<float>(in(xi,yi,1))/255 * y_range + min_y;
      float z = static_cast<float>(in(xi,yi,2))/255 * z_range + min_z;

      out(xi,yi,0) = x;
      out(xi,yi,1) = y;
      out(xi,yi,2) = z;
    }
  }
  return;
}

face3d::vertex_localizer::
vertex_localizer(triangle_mesh const& mesh, int cuda_device)
  : mesh_(mesh), cuda_device_(cuda_device)
{
  mesh_tree_.init(mesh_.V(), mesh_.F());
}

std::map<int, vgl_point_2d<double> >
face3d::vertex_localizer::
operator () ( dlib::array2d<vgl_point_3d<float> > const& semantic_map )
{
  std::map<int, vgl_point_2d<double>> vertex_locs;
  extract_vertex_projections( semantic_map, mesh_, vertex_locs,  mesh_tree_, cuda_device_ );
  return vertex_locs;
}
