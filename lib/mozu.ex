defmodule Mozu do
  @moduledoc """
  Documentation for `Mozu`.
  """

  alias Mozu.NIF

  @doc """
  Create mel filter bank.

  ## Examples

      iex> Mozu.mel_filter_bank(201, 80, 0.0, 8000.0, 16000, :slaney, true)
      %Npy{descr: "<f8", shape: {201, 80}, data: <<>>}

  """
  def mel_filter_bank(n_ferq_bins, n_mel_filters, min_freq, max_freq, sampling_rate, mel_scale \\ :htk, norm \\ false, triangularize_in_mel_space \\ false) do
    len = n_ferq_bins * n_mel_filters
    with {:ok, {^len, data}} <- NIF.mel_filter_bank(n_ferq_bins, n_mel_filters, min_freq, max_freq, sampling_rate, mel_scale, norm, triangularize_in_mel_space) do
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
  def hz2mel(freq, mel_scale \\ :htk)

  def hz2mel(%{__struct__: Npy, descr: "<f8", data: freq}=npy, mel_scale) do
    with {:ok, {_len, data}} <- NIF.hz2mel(freq, mel_scale),
      do: %{npy | data: data}
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
  def mel2hz(freq, mel_scale \\ :htk)

  def mel2hz(%{__struct__: Npy, descr: "<f8", data: mel}=npy, mel_scale) do
    with {:ok, {_len, data}} <- NIF.mel2hz(mel, mel_scale),
      do: %{npy | data: data}
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
end
