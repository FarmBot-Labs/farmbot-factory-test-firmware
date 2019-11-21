defmodule POST.ButtonTable do
  use GenServer

  @doc false
  def start_link(registry_name) do
    GenServer.start_link(__MODULE__, {__MODULE__, registry_name}, name: __MODULE__)
  end

  def subscribe(button) do
    registry = POST.Application.registry_name()
    {:ok, _} = Registry.register(registry, button, nil)
    :ok
  end

  @doc "put data"
  def put(table \\ __MODULE__, {button, status}) do
    GenServer.call(table, {:put, button, status})
  end

  @impl GenServer
  def init({table, registry_name}) do
    ^table = :ets.new(table, [:named_table, read_concurrency: true])

    state = %{table: table, registry: registry_name}
    {:ok, state}
  end

  @impl GenServer
  def handle_call({:put, button, status}, _from, state) do
    case :ets.lookup(state.table, button) do
      [{^button, ^status}] ->
        # No change, so no notifications
        :ok

      [{^button, old_status}] ->
        :ets.insert(state.table, {button, status})
        dispatch(state, button, old_status, status)

      [] ->
        :ets.insert(state.table, {button, status})
        dispatch(state, button, nil, status)
    end

    {:reply, :ok, state}
  end

  defp dispatch(state, button, old, new) do
    Registry.dispatch(state.registry, button, fn entries ->
      message = {:button, button, old, new}
      for {pid, _} <- entries, do: send(pid, message)
    end)

    :ok
  end
end
