-- Fluid dynamics configuration
fluid_dynamics = {
  iterations_density_diffusion = 2,
  iterations_velocity_diffusion = 5,
  iterations_pressure_equation = 20,
  velocity_advection_factor = 0.9,
  update_frequency = 30.0
}

display = {
  fluid_visual = 1.0
}

-- Configuration of debris components
debris = {

  status_component = {
    age_max = 20,
    sink_duration = 10
  },

  fluid_source_component = {
    density_back_projection = 1.0/fluid_dynamics.update_frequency,
    density_dynamic_r = 0.2,
    density_dynamic_g = 0.3,
    density_dynamic_b = 1.0,
    density_static_r = 1.0,
    density_static_g = 2.0,
    density_static_b = 5.0,
    velocity_back_projection = 1.0/fluid_dynamics.update_frequency
  }
}

-- Configuration of projectile components
projectile = {

  status_component = {
    age_max = 5,
    sink_duration = 5
  },

  fluid_source_component = {
    density_back_projection = 1.0/fluid_dynamics.update_frequency,
    density_dynamic_r = 1.0,
    density_dynamic_g = 1.0,
    density_dynamic_b = 0.5,
    density_static_r = 100.0,
    density_static_g = 20.0,
    density_static_b = 20.0,
    velocity_back_projection = 1.0/fluid_dynamics.update_frequency
  }
}
