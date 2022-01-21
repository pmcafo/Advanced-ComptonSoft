
#!/usr/bin/env ruby

require 'comptonsoft'

Degree = Math::PI / 180.0

class MyApp < ANL::ANLApp
  attr_accessor :inputs, :output
  attr_accessor :emin, :emax, :thetamin, :thetamax, :emin_si, :emax_si, :emin_cdte, :emax_cdte

  def setup()
    add_namespace ComptonSoft

    chain :ConstructDetector
    with_parameters(detector_configuration: "database/detector_configuration.xml")
    chain :ReadComptonEventTree
    with_parameters(file_list: inputs)
    chain :ComptonEventFilter
    with_parameters() do |m|
      m.define_condition
      m.add_hit_pattern("Si-CdTe")
      m.add_condition("E1+E2", @emin, @emax)
      m.add_condition("E1", @emin_si, @emax_si)
      m.add_condition("E2", @emin_cdte, @emax_cdte)
      m.add_condition("theta K", @thetamin, @thetamax)
    end
    chain :WriteComptonEventTree
    chain :SaveData
    with_parameters(output: output)
  end
end

app = MyApp.new
filename = "compton.root"
app.inputs = [filename]
app.output = filename.sub(".root", "_cut.root")
energy = 511.0 # keV
erange = 5.0 # keV
app.emin = energy - erange
app.emax = energy + erange
app.thetamin = 5.0 * Degree
app.thetamax = 175.0 * Degree
app.emin_si = 50.0
app.emax_si = 480.0
app.emin_cdte = 178.0
app.emax_cdte = app.emax
app.run(:all, 10000)