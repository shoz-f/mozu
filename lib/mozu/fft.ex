defmodule Mozu.FFT do

  alias Mozu.{Audio, NIF}

  @doc """
  """
  def fft(%Audio{channels: 1}=audio) do
    len = Audio.length(audio)
    with {:ok, {^len, spectrum}} <- NIF.fft(audio.wave) do
      %{
        __struct__: Npy,
        descr: "<c16",
        fortran_order: false,
        shape: {len},
        data: spectrum
      }
    end
  end
end
