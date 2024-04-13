defmodule Mozu.Wav do
  alias Mozu.{Audio, NIF}

  @doc """
  Load audio from WAV file.
  """
  def load(fname) do
    with {:ok, {channels, sampling, data}} <- NIF.wav_load(fname) do
      {:ok,
       %Audio{channels: channels, sampling: sampling, data: data }
      }
    end
  end

  @doc """
  Load audio from WAV file.
  """
  def load!(fname) do
    case load(fname) do
      {:ok, audio} -> audio
      any -> any
    end
  end

  @doc """
  Save auio to WAV file.
  """
  def save(%Audio{channels: channels, sampling: sampling, descr: "<f4", data: data}, fname) do
    NIF.wav_save(fname, channels, sampling, data);
  end
end
