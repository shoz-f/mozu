defmodule Mozu.Audio do
  alias Mozu.{NIF}

  @moduledoc """
  Audio structure.
  """
  defstruct channels: 1, sampling: 16000, wave: nil

  @doc """
  Load audio from file {.wav,}.
  """
  def load(path) do
    loader = case Path.extname(path) do
      ".wav" -> &NIF.wav_load/1
    end

    with {:ok, {channels, sampling, wave}} <- loader.(path) do
      {:ok, %__MODULE__{channels: channels, sampling: sampling, wave: wave}}
    end
  end

  @doc """
  Load audio from file {.wav}.
  """
  def load!(path) do
    with {:ok, audio} <- load(path) do
      audio
    end
  end

  @doc """
  Save auio to file {.wav,}.
  """
  def save(%__MODULE__{channels: channels, sampling: sampling, wave: wave}, path) do
    saver = case Path.extname(path) do
      ".wav" -> &NIF.wav_save/4
    end

    saver.(path, channels, sampling, wave)
  end

  @doc """
  Convert %Audio{} to %Npy{}.
  """
  def to_npy(%__MODULE__{channels: channels, sampling: sampling, wave: wave}) do
    length = div(byte_size(wave), 4)
    {
      %{
        __struct__: Npy,
        descr: "<f4",
        fortran_order: false,
        shape: (if channels > 1, do: {div(length, channels), channels}, else: {length}),
        data: wave
      },
      sampling
    }
  end

  @doc """
  Convert %Npy{} to %Audio{}.
  """
  def from_npy(%{descr: "<f4", shape: shape, data: data}, sampling) do
    channels = case shape do {_} -> 1; {_, ch} -> ch end
    %__MODULE__ {
      channels: channels,
      sampling: sampling,
      wave: data
    }
  end

  @doc """
  """
  def length(%__MODULE__{channels: channels, wave: data}) do
    div(byte_size(data), 4*channels)
  end

  @doc """
  Chunk %Audio{} into overlapping segments with parameters.
  """
  def to_frames(%__MODULE__{channels: 1, wave: wave}, hop \\ 160, window \\ 400, center \\ true) do
    with {:ok, {n, frames}} <- Mozu.NIF.to_frames(wave, hop, window, center), do:
      %{
        __struct__: Npy,
        descr: "<f4",
        fortran_order: false,
        shape: {n, window},
        data: frames
      }
  end

  @doc """
  """
  def take(%__MODULE__{channels: channels, wave: wave}=audio, len) do
    %__MODULE__{audio | wave: binary_slice(wave, 0, channels*4*len)}
  end
end
