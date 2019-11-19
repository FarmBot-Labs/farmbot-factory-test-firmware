defmodule POST.Comms do
  defmodule Framing do
    @behaviour Circuits.UART.Framing

    defmodule State do
      defstruct state: :get_opcode,
                op: nil,
                payload_length: nil,
                payload: <<>>
    end

    def init(_args) do
      {:ok, %State{state: :get_opcode}}
    end

    def add_framing(data, state) when is_binary(data) do
      # No processing - assume the app knows to send the right number of bytes
      {:ok, data, state}
    end

    def frame_timeout(state) do
      IO.puts("frame timeout!")
      # On a timeout, just return whatever was in the buffer
      {:ok, [{state.op, state.payload_length, state.payload}], %State{state: :get_opcode}}
    end

    def flush(:transmit, state) do
      # IO.inspect(state, label: "flush tx")
      state
    end

    def flush(:receive, state) do
      # IO.inspect(state, label: "flush rx")
      state
      # %State{state: :get_opcode}
    end

    def flush(:both, state) do
      # IO.inspect(state, label: "flush both")
      # %State{state: :get_opcode}
      state
    end

    def remove_framing(data, state) do
      process(data, state, [])
    end

    def process(<<op::integer-size(8), rest::binary>>, %State{state: :get_opcode} = state, m) do
      # IO.inspect(op, label: "got opcode")
      process(rest, %{state | state: :get_payload_length, op: op}, m)
    end

    def process(
          <<payload_length::integer-size(8), rest::binary>>,
          %State{state: :get_payload_length} = state,
          m
        ) do
      # IO.inspect(payload_length, label: "got payload length")
      process(rest, %{state | state: :get_payload, payload_length: payload_length}, m)
    end

    def process(<<char::binary-size(1), rest::binary>>, %State{state: :get_payload} = state, m) do
      new_state = %{state | payload: state.payload <> char}

      if byte_size(new_state.payload) == new_state.payload_length do
        # IO.inspect(new_state, label: "payload built")
        process(
          rest,
          %State{state: :get_opcode},
          m ++ [{new_state.op, new_state.payload_length, new_state.payload}]
        )
      else
        # IO.inspect(char, label: "buffering")
        process(rest, new_state, m)
      end
    end

    def process(<<>>, state, m) do
      if byte_size(state.payload) == state.payload_length do
        # IO.inspect(state, label: "payload built")
        {:ok, m ++ [{state.op, state.payload_length, state.payload}], %State{state: :get_opcode}}
      else
        # IO.inspect(state, label: "partial state (empty buffer)")
        {:in_frame, m, state}
      end
    end
  end

  alias Circuits.UART

  def test do
    {:ok, uart} = UART.start_link()
    :ok = UART.open(uart, "ttyUSB0", active: true, speed: 9600, framing: Framing)
    # :ok = UART.write(uart, <<114, 0>>)
    # :ok = ready(uart)
    Process.sleep(500)
    data = <<0x0::integer-size(8), 0x0::integer-size(8)>>
    :ok = UART.write(uart, data)
    do_recv(uart)
  end

  def test1 do
    {:ok, uart} = UART.start_link()
    :ok = UART.open(uart, "ttyUSB0", active: true, speed: 9600, framing: Framing)
    :ok = UART.write(uart, <<114, 0>>)
    UART.close(uart)
  end

  def move(axis, steps) do
    {:ok, uart} = UART.start_link()
    :ok = UART.open(uart, "ttyUSB0", active: true, speed: 9600, framing: Framing)
    :ok = UART.write(uart, <<114, 0>>)
    :ok = ready(uart)

    data =
      <<109::integer-size(8), 0x5::integer-size(8), axis::integer-size(8),
        steps::unsigned-integer-big-size(32)>>

    :ok = UART.write(uart, data)
    do_recv(uart)
  end

  def pin(number) do
    {:ok, uart} = UART.start_link()
    :ok = UART.open(uart, "ttyUSB0", active: true, speed: 9600, framing: Framing)
    :ok = ready(uart)
    data = <<112::integer-size(8), 0x1::integer-size(8), number::integer-size(8)>>
    :ok = UART.write(uart, data)
    do_recv(uart)
  end

  def ready(uart) do
    UART.flush(uart)

    receive do
      {_, _, {100, _, data}} ->
        debug_message = IO.iodata_to_binary(data)
        IO.write(:stdio, debug_message)
        ready(uart)

      {_, _, {0, 0, <<>>}} ->
        :ok

      {_, _, {114, _, _}} ->
        :ok

      error ->
        {:error, error}
    after
      5000 -> {:error, :timeout}
    end
  end

  def do_recv(uart) do
    UART.flush(uart)

    receive do
      {_, _, {100, _, data}} ->
        debug_message = IO.iodata_to_binary(data)
        IO.write(:stdio, debug_message)
        do_recv(uart)

      {_, _, {0, 0, <<>>}} ->
        UART.close(uart)
        :ok = GenServer.stop(uart, :normal)
        :ok
    after
      5000 ->
        IO.puts("timeout waiting for command to complete!")
        UART.close(uart)
        :ok = GenServer.stop(uart, :normal)
    end
  end
end
