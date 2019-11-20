defmodule POST.MixProject do
  use Mix.Project

  @app :post
  @version "0.1.0"
  @all_targets [:rpi, :rpi3]

  def project do
    [
      app: @app,
      version: @version,
      elixir: "~> 1.9",
      archives: [nerves_bootstrap: "~> 1.6"],
      start_permanent: Mix.env() == :prod,
      build_embedded: true,
      aliases: [loadconfig: [&bootstrap/1]],
      deps: deps(),
      elixirc_paths: elixirc_paths(Mix.env(), Mix.target()),
      compilers: [:elixir_make | Mix.compilers()],
      make_targets: ["all"],
      make_clean: ["clean"],
      make_error_message: "Could not build firmware source",
      make_env: %{"MIX_TARGET" => to_string(Mix.target())},
      dialyzer: dialyzer(),
      releases: [{@app, release()}],
      preferred_cli_target: [run: :host, test: :host]
    ]
  end

  # Starting nerves_bootstrap adds the required aliases to Mix.Project.config()
  # Aliases are only added if MIX_TARGET is set.
  def bootstrap(args) do
    Application.start(:nerves_bootstrap)
    Mix.Task.run("loadconfig", args)
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      mod: {POST.Application, []},
      extra_applications: [:logger, :runtime_tools]
    ]
  end

  defp elixirc_paths(_, :host), do: ["./lib"]
  defp elixirc_paths(_, target) when target in @all_targets, do: ["./lib", "./platform"]

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      # Dependencies for all targets
      {:nerves, "~> 1.5.0", runtime: false},
      {:shoehorn, "~> 0.6"},
      {:ring_logger, "~> 0.6"},
      {:toolshed, "~> 0.2"},
      {:dialyxir, "~> 1.0.0-rc.6", only: [:dev, :test], runtime: false},
      {:circuits_uart, "~> 1.3"},
      {:elixir_make, "~> 0.6.0", runtime: false},

      # Dependencies for all targets except :host
      {:nerves_runtime, "~> 0.10", targets: @all_targets},
      {:busybox, "~> 0.1", targets: @all_targets},
      {:vintage_net, "~> 0.6.0", targets: @all_targets},
      {:nerves_firmware_ssh, "~> 0.2", targets: @all_targets},
      {:nerves_time, "~> 0.2", targets: @all_targets},
      {:mdns_lite, "~> 0.4", targets: @all_targets},

      # Dependencies for specific targets
      {:nerves_system_br, "~> 1.9.5", runtime: false, targets: @all_targets, override: true},
      {:farmbot_system_rpi, "~> 1.9.0-farmbot.1", runtime: false, targets: :rpi},
      {:farmbot_system_rpi3, "~> 1.9.1-farmbot.2", runtime: false, targets: :rpi3}
    ]
  end

  def release do
    [
      overwrite: true,
      cookie: "democookie",
      include_erts: &Nerves.Release.erts/0,
      steps: [&Nerves.Release.init/1, :assemble],
      strip_beams: false
    ]
  end

  defp dialyzer() do
    [
      flags: [:race_conditions, :unmatched_returns, :error_handling, :underspecs]
    ]
  end
end
