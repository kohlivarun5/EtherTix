let text = ReasonReact.string;
let int(i) = i |> string_of_int |> ReasonReact.string;

type data = {
  numSold:int,
  numUnsold:int
}

type state = {
  web3 : Web3.state,
  address : BsWeb3.Eth.address,
  event: Event.t,
  data:data
};
type action = | Submit | Data(data)

let component = ReasonReact.reducerComponent("EventView");

let make = (~web3,~address,~event,_children) => {
  ...component,
  initialState: () => { web3:web3,address:address,event:event,data:{numSold:0,numUnsold:0}},
  didMount: ({send,state}) => { 
    Js.Promise.all2(
      (
        (Event.numSold(state.event)
        |> BsWeb3.Eth.call),
        (Event.numUnSold(state.event)
        |> BsWeb3.Eth.call)
      )
    )
    |> Js.Promise.then_ ( ((numSold,numUnsold)) => 
        send(Data({numSold:numSold,numUnsold:numUnsold}))
        |> Js.Promise.resolve);
    () 
  },
  reducer: (action,state:state) => {
    switch (action) {
    | Submit => ReasonReact.NoUpdate 
    | Data(data) => ReasonReact.Update({...state,data:data})
    }
  },
  render: ({send,state}) =>
<div className="card mb-3">
  <div className="row card-body">
    <div className="col">
      <h5 className="card-title">(text("Sold"))</h5>
      <h6 className="card-subtitle text-muted">(int(state.data.numSold))</h6>
    </div>
    <div className="col">
      <h5 className="card-title">(text("Unsold"))</h5>
      <h6 className="card-subtitle text-muted">(int(state.data.numUnsold))</h6>
    </div>
  </div>
  <div className="row card-body"> 
    <div className="col">(text("Cras justo odio"))</div>
    <div className="col">(text("Cras justo odio"))</div>
  </div>
  <div className="card-footer text-muted">(text("2 days ago"))</div>
</div>
};
