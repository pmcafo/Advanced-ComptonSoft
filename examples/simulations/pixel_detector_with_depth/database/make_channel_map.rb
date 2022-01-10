#!/usr/bin/env ruby

# author: Hirokazu Odaka
# 2016-08-25 | for ComptonSoft version 5

require 'csv'
require 'builder'

def detector_node(xml, detector_prefix)
  xml.detector(prefix: detector_prefix,
               type: "2DPixel",
               num_sections: 4,
               num_channels: 64,
               num_x: 16,
               num_y: 16) do
    n = 64
    n.times{|i| xml.channel(section: 0, index: i, x: (15-i/8), y: (8+i%8))  }
    n.times{|i| xml.channel(section: 1,