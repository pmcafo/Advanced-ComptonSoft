# comptonsoft/simulation.rb
#
# @author Hirokazu Odaka, Yuto Ichinohe
#

module ComptonSoft
  include AnlGeant4

  # Calculate noise parameter 1 from energy resolution ratio (Delta E/E) at energy E.
  #
  # @param [Float] energy energy [keV]
  # @param [Float] ratio ratio of energy resolution to energy, Delta E over E
  # @return [Float] noise parameter 1
  #
  def noise_param1(energy, ratio)
    ratio * Math.sqrt(energy)
  end
  module_function :noise_param1

  module DetectorType
    Pad = 1
    DoubleSidedStrip = 2
    Scintillator = 3
  end

  # Simulation application class inherited from ANLApp
  #
  # @author Hirokazu Odaka
  # @author Yuto Ichinohe
  # @date 2016-08-25 (latest) | H. Odaka
  # @date 2020-04-03 | H. Odaka | support multiple channel properties database
  #
  class Simulation < ANL::ANLApp
    def initialize()
      super

      ### Basic settings
      @setup_mode = :normal

      ### Input files
    