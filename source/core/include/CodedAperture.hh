
/*************************************************************************
 *                                                                       *
 * Copyright (c) 2019 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
 *************************************************************************/
/**
 * CodedAperture
 *
 * @author Tsubasa Tamba, Satoshi Takashima, Hirokazu Odaka
 * @date 2019-11-01
 * @date 2019-11-15 | H. Odaka | redesign
 * @date 2020-10-27 | T. Tamba | redesign
 */
#ifndef COMPTONSOFT_CodedAperture_H
#define COMPTONSOFT_CodedAperture_H 1

#include "VCodedAperture.hh"
#include <random>
#include "CSTypes.hh"

namespace comptonsoft {

class CodedAperture : public VCodedAperture
{
public:
  CodedAperture();
  virtual ~CodedAperture();

  void setElementSizes(double detector_x,
                       double detector_y,
                       double aperture_x,
                       double aperture_y)
  {
    detector_element_size_x_ = detector_x;
    detector_element_size_y_ = detector_y;
    aperture_element_size_x_ = aperture_x;
    aperture_element_size_y_ = aperture_y;
  }

  void setAperturePattern(const std::shared_ptr<image_t>& pattern) override;
  void setEncodedImage(const std::shared_ptr<image_t>& image) override;
  bool buildSkyImage() override;

  void decode() override;

  std::shared_ptr<image_t> DecodedImage() const override { return decoded_image_; }

  void setSkyNum(int x, int y) { sky_num_x_ = x; sky_num_y_ = y; }
  int SkyNumX() const { return sky_num_x_; }
  int SkyNumY() const { return sky_num_y_; }
  void setSkyFov(double x, double y) { sky_fov_x_ = x; sky_fov_y_ = y; }
  double SkyFovX() const { return sky_fov_x_; }
  double SkyFovY() const { return sky_fov_y_; }
  void setDetectorToApertureDistance(double v) { detector_to_aperture_distance_ = v; }
  double DetectorToApertureDistance() const { return detector_to_aperture_distance_; }

  double SkyElementAngleX() const override { return sky_element_angle_x_; }
  double SkyElementAngleY() const override { return sky_element_angle_y_; }

  void setDetectorRollAngle(double v) { detector_roll_angle_ = v; }
  double DetectorRollAngle() const { return detector_roll_angle_; }
  void setApertureRollAngle(double v) { aperture_roll_angle_ = v; }
  double ApertureRollAngle() const { return aperture_roll_angle_; }
  void setApertureOffset(double x, double y) { aperture_offset_.setX(x); aperture_offset_.setY(y); }
  vector2_t ApertureOffset() const { return aperture_offset_; }
  void setSkyOffset(double x, double y) { sky_offset_.setX(x); sky_offset_.setY(y); }
  vector2_t SkyOffset() const { return sky_offset_; }
  void setNumDecodingIterations(int v) { num_decoding_iterations_ = v; }
  int NumDecodingIterations() const { return num_decoding_iterations_; }
  void setDecodingMode(int v) { decoding_mode_ = v; }
  int DecodingMode() const { return decoding_mode_; }
  
  struct ID {
    int ix = 0;
    int iy = 0;
    bool invalid = false;
    ID() {}
    ID(int ix, int iy, bool invalid=false): ix(ix), iy(iy), invalid(invalid) {}
    ID(bool invalid): invalid(invalid) {}
  };

  ID DetectorID(const vector2_t& v) const;
  ID ApertureID(const vector2_t& v) const;
  ID SkyID(const vector2_t& v) const;
  vector2_t DetectorPosition(const ID& id, bool random = false);
  vector2_t AperturePosition(const ID& id, bool random = false);
  vector2_t SkyAngle(const ID& id, bool random = false);
  
  vector2_t DetectorToAperture(vector2_t v);
  vector2_t ApertureToDetector(vector2_t v);
  vector2_t DetectedAngleToSkyAngle(vector2_t v);
  vector2_t SkyAngleToDetectedAngle(vector2_t v);

  double UniformDistribution(double x1, double x2);

private:
  double detector_element_size_x_ = 1.0;
  double detector_element_size_y_ = 1.0;
  double aperture_element_size_x_ = 1.0;
  double aperture_element_size_y_ = 1.0;
  double sky_element_angle_x_ = 1.0;
  double sky_element_angle_y_ = 1.0;

  int sky_num_x_ = 0;
  int sky_num_y_ = 0;
  double sky_fov_x_ = 0.0;
  double sky_fov_y_ = 0.0;
  double detector_to_aperture_distance_ = 1.0;

  double detector_roll_angle_ = 0.0;
  double aperture_roll_angle_ = 0.0;
  vector2_t aperture_offset_ = vector2_t(0.0, 0.0);
  vector2_t sky_offset_ = vector2_t(0.0, 0.0);
  int num_decoding_iterations_ = 1;
  int decoding_mode_ = 1;

  std::shared_ptr<image_t> aperture_pattern_;
  std::shared_ptr<image_t> decoder_array_;
  std::shared_ptr<image_t> encoded_image_;
  std::shared_ptr<image_t> decoded_image_;

  void decode_based_on_sky_coordinate();
  void decode_based_on_aperture_coordinate();

  std::mt19937 randomGenerator_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_CodedAperture_H */