defmodule Mozu do
  @moduledoc """
  Documentation for `Mozu`.
  """

  @doc """
  Create mel filter bank.

  ## Examples

      iex> Mozu.mel_filter_bank(201, 80, 0.0, 8000.0, 16000, :slaney, true)
      %Npy{descr: "<f8", shape: {201, 80}, data: <<>>}

  """
  def mel_filter_bank(n_ferq_bins, n_mel_filters, min_freq, max_freq, sampling_rate, mel_scale \\ :htk, norm \\ false, triangularize_in_mel_space \\ false) do
    with {:ok, data} <- Mozu.NIF.mel_filter_bank(n_ferq_bins, n_mel_filters, min_freq, max_freq, sampling_rate, mel_scale, norm, triangularize_in_mel_space) do
      %{
        __struct__: Npy,
        descr: "<f8",
        fortran_order: false,
        shape: {n_ferq_bins, n_mel_filters},
        data: data
      }
    end
  end

  @doc """
  Convert frequency from hertz to mels.

  ## Examples

      iex> Mozu.hz2mel(freq, :slaney)
      mel

  """
  def hz2mel(%{data: freq}=npy, mel_scale) do
    with {:ok, data} <- Mozu.NIF.hz2mel(freq, mel_scale), do:
      %{npy | data: data}
  end

  def hz2mel(freq, mel_scale) do
    case mel_scale do
      :htk    -> (2595.0*:math.log10(1.0 + freq/700.0))
      :kaldi  -> (1127.0*:math.log(1.0 + freq/700.0))
      :slaney -> if freq >= 1000.0,
                   do:   (15.0 + :math.log(freq/1000.0)*(27.0/:math.log(6.4))),
                   else: (3.0*freq/200.0)
    end
  end

  @doc """
  Convert frequency from mels to hertz.

  ### Examples

      iex> Mozu.mel2hz(mel, :slaney)
      freq

  """
  def mel2hz(%{data: mel}=npy, mel_scale) do
    with {:ok, data} <- Mozu.NIF.mel2hz(mel, mel_scale), do:
      %{npy | data: data}
  end

  def mel2hz(mel, mel_scale) do
    case mel_scale do
      :htk    -> (700.0*(:math.pow(10.0, mel/2595.0) - 1.0))
      :kaldi  -> (700.0*(:math.exp(mel/1127.0) - 1.0))
      :slaney -> if mel >= 15.0,
                   do:   (1000.0*:math.exp((:math.log(6.4)/27.0)*(mel - 15.0))),
                   else: (200.0*mel/3.0)
    end
  end

  @doc """
  Generates linearly spaced points between a specified start and stop value.

  ### Examples

      iex> linspace(2.0, 3.0, 5)
      %Npy{shape: {5}, <<>>}  # 2.0, 2.2, 2.4, 2.6, 2.8, 3.0}

  """
  def linspace(start, stop, num, endpoint \\ true) do
    with {:ok, data} <- Mozu.NIF.linspace(start, stop, num, endpoint), do:
      %{
        __struct__: Npy,
        descr: "<f8",
        fortran_order: false,
        shape: {num},
        data: data
      }
  end

  def sample() do
    Mozu.Audio.load!("ds0.wav") |> Mozu.Audio.take(100)
  end
end
