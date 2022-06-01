defmodule POST.MixProject do
  use Mix.Project

  @app :post
  @version "0.2.0"
  @all_targets [:rpi02w, :rpi4]

  def project do
    [
      app: @app,
      version: @version,
      elixir: "~> 1.13",
      archives: [nerves_bootstrap: "~> 1.10"],
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
      {:nerves, "~> 1.7", runtime: false},
      {:elixir_make, "~> 0.6", runtime: false},
      {:shoehorn, "~> 0.9"},
      {:ring_logger, "~> 0.8"},
      {:toolshed, "~> 0.2"},
      {:dialyxir, "~> 1.1", only: [:dev, :test], runtime: false},
      {:circuits_uart, "~> 1.4"},
      {:muontrap, "~> 1.0"},

      # Dependencies for all targets except :host
      {:nerves_runtime, "~> 0.11", targets: @all_targets},
      {:busybox, "~> 0.1", targets: @all_targets},
      {:nerves_time, "~> 0.4", targets: @all_targets},
      {:circuits_gpio, "~> 1.0", targets: @all_targets},

      # Dependencies for specific targets
      {:nerves_system_br, "~> 1.18", runtime: false, targets: @all_targets, override: true},
      {:farmbot_system_rpi3,
       git: "https://github.com/FarmBot/farmbot_system_rpi3",
       tag: "v1.18.1-farmbot.1",
       runtime: false,
       targets: :rpi02w},
      {:farmbot_system_rpi4,
       git: "https://github.com/FarmBot/farmbot_system_rpi4",
       tag: "v1.18.1-farmbot.1",
       runtime: false,
       targets: :rpi4}
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
