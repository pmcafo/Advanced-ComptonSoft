#!/usr/bin/env ruby

require 'comptonsoft'

class MyApp < ANL::ANLApp
  attr_accessor :inputs, :output

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ConstructDetector
    with_parameters(detector_configuration: "database/detector_configuration.xml",
                    verbose_level: 1)
    chain :ReadHitTree
    with_parameters(file_list: @inputs)
    chain :EventReconstruction
    with_parameters(max_hits: 1,
                    reconstruction_method: "photoabsorption",
                    source_distant: true,
                    source_direction: vec(0.0, 0.0, 1.0))
    chain :HistogramEnergySpectrum
    with_parameters(number_of_bins: 2048,
                    energy_min: 50.0,
                    energy_max: 20000.0,
                    event_selections: ["HitPattern:1hit"])
    chain :SaveData
    with_parameters(output: @output)
  end
end

a = MyApp.new
a.inputs = ["simulation_line.root"]
a.output = "spectrum.root"
a.run(:all, 100000)
