defmodule POST.Comms.FlashFirmware do
  @moduledoc "Task to flash firmware on boot"
  alias POST.Comms

  defmodule Reset do
    @moduledoc "Behaviour for platform specific bootloader reset mode"
    @doc "Reset the firmware line into bootloader mode"
    @callback reset() :: any()
  end

  @firmware_reset_impl Application.get_env(:post, __MODULE__)[:reset]
  @firmware_reset_impl ||
    Mix.raise("""
    Firmware reset implementation not specified.
    """)

  @doc false
  def child_spec(opts) do
    %{
      id: __MODULE__,
      start: {__MODULE__, :flash_firmware, [opts]}
    }
  end

  def force do
    Avrdude.flash(firmware_file(), Comms.serial_port(), &@firmware_reset_impl.reset/0)
  end

  def flash_firmware(_) do
    if Comms.serial_port() do
      case Comms.test() do
        :error ->
          _ = Avrdude.flash(firmware_file(), Comms.serial_port(), &@firmware_reset_impl.reset/0)
          _ = Comms.test()
          :ignore

        {:ok, _} ->
          :ignore
      end
    end
  end

  def firmware_file do
    Application.app_dir(:post, ["priv", "firmware.hex"])
  end
end
