defmodule POST.NullButton do
  use GenServer

  def trigger(id) do
    GenServer.call(name(id), :trigger)
  end

  def start_link(id) do
    GenServer.start_link(__MODULE__, id, name: name(id))
  end

  def init(id) do
    {:ok, id}
  end

  def handle_call(:trigger, _from, id) do
    POST.ButtonTable.put({id, 1})
    POST.ButtonTable.put({id, 0})
    {:reply, :ok, id}
  end

  def name(id) do
    Module.concat(__MODULE__, to_string(id))
  end
end
