defmodule POST.Comms.Framing do
  @moduledoc """
  Implementation of the Framing behaviour for the dirty
  comms system for this application.

  Messages are shaped like:

      <<opcode::1, payload_size::1, payload::payload_size>>

  there are currently 3 implemented opcodes:

  * `0x00` - noop. Sent at the end of every completed command
  * `0x6D` - movement. Moves an axis a number of steps
    * argv[0] uint8 axis id. (0-3)
    * argv[1-3] uint32 number of steps to move
  * `0x70` - pin control. Turns a pin for 300 ms, and back off again
    * argv[0] uint8 pin to control.
  * `0x64` - debug messages from the arduino.
    * argv cstring human readable message that can be wrote to stdio
  """
  @behaviour Circuits.UART.Framing

  defmodule State do
    @moduledoc false
    defstruct state: :get_opcode,
              op: nil,
              payload_length: nil,
              payload: <<>>
  end

  @impl Circuits.UART.Framing
  def init(_args) do
    {:ok, %State{state: :get_opcode}}
  end

  @impl Circuits.UART.Framing
  def add_framing(data, state) when is_binary(data) do
    # No processing - assume the app knows to send the right number of bytes
    {:ok, data, state}
  end

  @impl Circuits.UART.Framing
  def frame_timeout(state) do
    IO.puts("frame timeout!")
    # On a timeout, just return whatever was in the buffer
    {:ok, [{state.op, state.payload_length, state.payload}], %State{state: :get_opcode}}
  end

  @impl Circuits.UART.Framing
  def flush(:transmit, state), do: state
  def flush(:receive, state), do: state
  def flush(:both, state), do: state

  @impl Circuits.UART.Framing
  def remove_framing(data, state), do: process(data, state, [])

  def process(<<op::integer-size(8), rest::binary>>, %State{state: :get_opcode} = state, m) do
    process(rest, %{state | state: :get_payload_length, op: op}, m)
  end

  def process(
        <<payload_length::integer-size(8), rest::binary>>,
        %State{state: :get_payload_length} = state,
        m
      ) do
    process(rest, %{state | state: :get_payload, payload_length: payload_length}, m)
  end

  def process(<<char::binary-size(1), rest::binary>>, %State{state: :get_payload} = state, m) do
    new_state = %{state | payload: state.payload <> char}

    if byte_size(new_state.payload) == new_state.payload_length do
      process(
        rest,
        %State{state: :get_opcode},
        m ++ [{new_state.op, new_state.payload_length, new_state.payload}]
      )
    else
      process(rest, new_state, m)
    end
  end

  def process(<<>>, state, m) do
    if byte_size(state.payload) == state.payload_length do
      {:ok, m ++ [{state.op, state.payload_length, state.payload}], %State{state: :get_opcode}}
    else
      # IO.inspect(state, label: "in frame")
      {:in_frame, m, state}
    end
  end
end
