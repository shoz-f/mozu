defmodule MozuTest do
  use ExUnit.Case
  doctest Mozu

  test "greets the world" do
    assert Mozu.hello() == :world
  end
end
