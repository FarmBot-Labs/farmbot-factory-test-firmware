defmodule POST.Comms.FlashFirmware.NullReset do
  @moduledoc "Reset impl that does nothing"
  @behaviour POST.Comms.FlashFirmware.Reset

  @impl POST.Comms.FlashFirmware.Reset
  def reset(), do: :ok
end
