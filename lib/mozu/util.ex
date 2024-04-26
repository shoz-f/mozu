defmodule Mozu.Util do
  alias Mozu.NIF

  @doc """
  """
  def astype(%{__struct__: Npy, descr: type}=npy, astype) when type == astype,
    do: npy

  def astype(%{__struct__: Npy, descr: type, data: data}=npy, astype) do
    {:ok, {_len, typed}} = case type do
      "<f4" -> NIF.f4_astype(data, astype)
      "<f8" -> NIF.f8_astype(data, astype)
      "<c8" -> NIF.c8_astype(data, astype)
      "<c16" -> NIF.c16_astype(data, astype)
    end

    %{npy | descr: astype, data: typed}
  end

  @doc """
  Generates linearly spaced points between a specified start and stop value.

  ### Examples

      iex> linspace(2.0, 3.0, 5)
      %Npy{shape: {5}, <<>>}  # 2.0, 2.2, 2.4, 2.6, 2.8, 3.0}

  """
  def linspace(start, stop, num, endpoint \\ true) do
    with {:ok, {^num, data}} <- NIF.linspace(start, stop, num, endpoint), do:
      %{
        __struct__: Npy,
        descr: "<f8",
        fortran_order: false,
        shape: {num},
        data: data
      }
  end
end
