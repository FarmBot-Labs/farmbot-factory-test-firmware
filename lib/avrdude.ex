defmodule Avrdude do
  @moduledoc """
  It's AVR, my dudes.
  """

  @uart_speed 115_200

  @spec flash(Path.t(), Path.t()) :: {number, any()}
  def flash(hex_path, tty_path) do
    tty_path =
      if String.contains?(tty_path, "/dev") do
        tty_path
      else
        "/dev/#{tty_path}"
      end

    _ = File.stat!(hex_path)

    # STEP 1: Is the UART in use?
    args = [
      "-patmega2560",
      "-cwiring",
      "-P#{tty_path}",
      "-b#{@uart_speed}",
      "-D",
      "-V",
      "-Uflash:w:#{hex_path}:i"
    ]

    MuonTrap.cmd("avrdude", args, into: IO.stream(:stdio, :line))
  end
end
