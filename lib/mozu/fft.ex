defmodule Mozu.FFT do

  alias Mozu.{Audio, NIF}

  @doc """
  """
  def rfft(data, opts \\ [])
  def rfft(%Audio{channels: 1, wave: wave}, opts),
    do: rfft_sub(wave, opts)
  def rfft(%{__struct__: Npy, descr: "<f4", shape: {_}, data: data}, opts),
    do: rfft_sub(data, opts)

  defp rfft_sub(data, opts) when is_binary(data) do
    power   = Keyword.get(opts, :power,  nil)
    oneside = Keyword.get(opts, :oneside, true)

    with {:ok, {len, rfft}} <- NIF.rfft_1D(data, power, oneside) do
      %{
        __struct__: Npy,
        descr: case power do :abs -> "<f8";  :norm -> "<f8";  _else -> "<c16" end,
        fortran_order: false,
        shape: {len},
        data: rfft
      }
    end
  end

  @doc """
  """
  def power(%{__struct__: Npy, descr: "<c16", data: data}, power) when power in [:abs, :norm] do
    with {:ok, {len, power}} <- NIF.power(data, power) do
      %{
        __struct__: Npy,
        descr: "<f8",
        fortran_order: false,
        shape: {len},
        data: power
      }
    end
  end
end
