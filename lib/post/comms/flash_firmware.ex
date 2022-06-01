defmodule POST.Comms.FlashFirmware do
  @moduledoc "Task to flash firmware on boot"
  alias POST.Comms

  @doc false
  def child_spec(opts) do
    %{
      id: __MODULE__,
      start: {__MODULE__, :flash_firmware, [opts]}
    }
  end

  def force do
    Avrdude.flash(firmware_file(), Comms.serial_port())
  end

  def flash_firmware(_) do
    if Comms.serial_port() do
      case Comms.test() do
        :error ->
          _ = Avrdude.flash(firmware_file(), Comms.serial_port())
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
