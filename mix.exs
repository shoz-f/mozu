defmodule Mozu.MixProject do
  use Mix.Project

  def project do
    [
      app: :mozu,
      version: "0.1.0",
      elixir: "~> 1.14",
      start_permanent: Mix.env() == :prod,
      make_executable: "make",
      make_clean: ["clean"],
      compilers: [:elixir_make] ++ Mix.compilers(),
      deps: deps(),

      description: description(),
      package: package(),
      source_url: "https://github.com/shoz-f/mozu.git",

      docs: docs()
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger]
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:elixir_make, "~> 0.8.3"},
  #    {:npy, path: "../npy_ex"},

      {:ex_doc, "~> 0.24", only: :dev, runtime: false},
      {:npy, path: "../npy_ex"}
    ]
  end

  defp description() do
    "audio processing functions."
  end

  defp package() do
    [
      name: "mozu",
      licenses: ["Apache-2.0"],
      links: %{"GitHub" => "https://github.com/shoz-f/mozu.git"},
      files: ~w(lib mix.exs README* CHANGELOG* LICENSE* Makefile src/*.{cc,h,inc} src/3rd_party/dr_libs/dr_wav.h)
    ]
  end

  defp docs do
    [
      main: "readme",
      extras: [
        "README.md",
        "CHANGELOG.md"
      ],
    ]
  end

end
