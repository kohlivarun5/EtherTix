let text = ReasonReact.string;
let int(i) = i |> string_of_int |> ReasonReact.string;

type buy_data = {
  numTickets:int,
  totalCost:int
}

type state = {
  web3 : Web3.state,
  event_address:BsWeb3.Eth.address,
  event : option(Event.t),
  buy_data:buy_data
};

type action = 
| EventAddress(BsWeb3.Eth.address)
| NumTickets(int)
| TotalCost(int)
| SubmitBuy

let component = ReasonReact.reducerComponent("UserView");

let make = (~web3,_children) => {
  ...component,
  initialState: () => { 
    web3:web3,
    event_address:"",
    event:None,
    buy_data:{numTickets:1,totalCost:0}
  },
  reducer: (action,state:state) => {
    switch (action) {
    | EventAddress(address) => 
        let event = Event.ofAddress(state.web3.web3,address);
        ReasonReact.UpdateWithSideEffects({...state,event_address:address,event:Some(event)},(self) => 
          self.send(NumTickets(state.buy_data.numTickets))
        )
    | NumTickets(numTickets) => 
        Js.log(numTickets);
        Js.log(state);
        ReasonReact.UpdateWithSideEffects({...state,buy_data:{...state.buy_data,numTickets}},(self) => {
          Event.getCostFor(Js.Option.getExn(state.event),~numTickets)
          |> BsWeb3.Eth.call 
          |> Js.Promise.then_ ((totalCost) => { Js.log(totalCost); self.send(TotalCost(totalCost)) |> Js.Promise.resolve});
          ()
        })
    | TotalCost(totalCost) => 
        Js.log(state);
        ReasonReact.Update({...state,buy_data:{...state.buy_data,totalCost}})
    | SubmitBuy => ReasonReact.UpdateWithSideEffects(state,(self) => {
        Js.log(state);
        Event.buy(Js.Option.getExn(state.event),~numTickets=state.buy_data.numTickets)
        |> BsWeb3.Eth.send(
            BsWeb3.Eth.make_transaction_with_value(
              ~value=state.buy_data.totalCost,~from=state.web3.account))
        |> Js.Promise.then_ (_ => Js.Promise.resolve());
        ()
      })
    }
  },
  render: ({send,state}) =>
<div className="card">

  <h5 className="card-header">(text("Buy Tickets"))</h5>
  <div className="card-body padding-vertical-less"> 
    <div className="form-group" style=(ReactDOMRe.Style.make(~margin="3%",()))>
      <div className="row">
        <label className="col col-5 col-form-label text-muted">(text("Event"))</label>
        <input className="col form-control" type_="text" placeholder="" id="inputLarge"
               onChange=(event => send(EventAddress(ReactEvent.Form.target(event)##value)))
               value=state.event_address
        />
      </div>
      <div className="row">
        <label className="col col-5 col-form-label text-muted">(text("Number of tickets"))</label>
        <input className="col form-control" type_="text" placeholder="" id="inputLarge"
               onChange=(event => send(NumTickets(int_of_float(Js.Float.fromString(ReactEvent.Form.target(event)##value)))))
               value=(string_of_int(state.buy_data.numTickets))
        />
      </div>
      <div className="row">
        <label className="col col-5 col-form-label text-muted">(text("Total Cost (wei)"))</label>
        <input className="col form-control" type_="text" placeholder="" id="inputLarge" 
               onChange=(event => send(TotalCost(ReactEvent.Form.target(event)##value)))
               value=(string_of_int(state.buy_data.totalCost))
        />
      </div>
      <div className="row">
        <button className="btn btn-success" onClick=(_ => send(SubmitBuy))
                style=(ReactDOMRe.Style.make(~marginLeft="20px",~marginRight="20px",~marginTop="20px",~width="100%",())) >
          (text("Submit"))
        </button>
      </div>
    </div>
  </div>


</div>
};

