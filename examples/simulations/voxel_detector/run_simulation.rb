
#! /usr/bin/env ruby

require 'comptonsoft'

def run_simulation(num, random, output)
  energy = 1332.5 # keV
  
  sim = ComptonSoft::Simulation.new
  sim.output = output
  sim.random_seed = random
  sim.verbose = 0
  sim.print_detector_info
  sim.set_database(detector_configuration: "database/detector_configuration.xml",
                   detector_parameters: "database/detector_parameters.xml")
  sim.set_gdml "database/mass_model.gdml"
  sim.set_physics(hadron_hp: false, cut_value: 0.001)

  sim.set_primary_generator :PlaneWavePrimaryGen, {
    particle: "gamma",
    photon_index: 0.0,
    energy_min: energy,
    energy_max: energy,
    position: vec(0.0, 0.0, 50.0),
    direction: vec(0.0, 0.0, -1.0),
    radius: 10.0
  }

  sim.run(num)
end

### main ###
num = 100000
output = "simulation.root"
random = 0
run_simulation(num, random, output)