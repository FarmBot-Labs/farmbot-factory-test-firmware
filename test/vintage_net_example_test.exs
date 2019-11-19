defmodule POSTTest do
  use ExUnit.Case
  doctest POST

  test "greets the world" do
    assert POST.hello() == :world
  end
end
