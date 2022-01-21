#! /usr/bin/env ruby
require 'comptonsoft'

def run_simulation(num, random, output)
  energy_min = 50.0 # keV
  energy_max = 20000.0 # keV

  sim = ComptonSoft::Simulation.new
  sim.output = output
  sim.random_seed = random
  sim.verbose = 0
  sim.print_detector_info
  sim.set_database(detector_configuration: "database/detector_configuration.xml",
                   detector_parameters: "database/detector_parameters.xml")
  sim.set_gdml "database/mass_model.gdml"
  sim.set_physics(hadron_hp: false, cut_value: 0.001)

  sim.set_primary_generator :PlaneWaveRectanglePrimaryGen, {
    particle: "gamma",
    photon_index: 0.0,
    energy_min: energy_min,
    energy_max: energy_max,
    position: vec(0.0, 0.0, 10.0),
    direction: vec(0.0, 0.0, -1.0),
    size_x: 5.0,
    size_y: 5.0,
    roll_angle: 0.0,
  }

  sim.run(num)
end

### main ###

num = 1000000
runs = (1..12).to_a
a = ANL::ParallelRun.new
a.num_processes = 4
a.set_log "simulation_%03d.log"
a.run(runs, testrun: true) do |run_id|
  random = run_id
  output = "simulation_%03d.root" % run_id
  run_simulation(num, random, output)
end
