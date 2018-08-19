let text = ReasonReact.string;
let int(i) = i |> string_of_int |> ReasonReact.string;

type buy_data = {
  event:Event.t,
  numTickets:int,
  totalCost:BsWeb3.Types.big_number
}

type event_data = {
  address:BsWeb3.Eth.address,
  event:Event.t,
  description : string,
  tickets:Js.Array.t(int)
}

type state = {
  web3 : Web3.state,
  event_address:BsWeb3.Eth.address,
  buy_data:option(buy_data),
  myEvents:Js.Array.t(event_data)
};

type action = 
| GetMyEvents 
| AddEvent(BsWeb3.Eth.address)
| MyEventData(event_data)
| BuyEventAddress(BsWeb3.Eth.address)
| NumTickets(int)
| TotalCost(BsWeb3.Types.big_number)
| SubmitBuy

let component = ReasonReact.reducerComponent("UserView");

let make = (~web3,_children) => {
  ...component,
  initialState: () => { 
    web3:web3,
    event_address:"",
    buy_data:None,
    myEvents:[||]
  },
  didMount: self => { self.send(GetMyEvents) },
  reducer: (action,state:state) => {
    switch (action) {
    | GetMyEvents => {
        ReasonReact.UpdateWithSideEffects(state, (self) => {
          let transaction_data = BsWeb3.Eth.make_transaction(~from=state.web3.account);
          Universe.userEvents(state.web3.universe)
          |> BsWeb3.Eth.call_with(transaction_data)
          |> Js.Promise.then_ ((events_addr:Js.Array.t(BsWeb3.Eth.address)) => {
              events_addr 
              |> Js.Array.map((addr) => { self.send(AddEvent(addr)) })
              |> Js.Promise.resolve
            });
          ()
        })
      }
    | AddEvent(address) => {
        ReasonReact.UpdateWithSideEffects(state,(self) => {
          let event = Event.ofAddress(state.web3.web3,address);
          let transaction_data = BsWeb3.Eth.make_transaction(~from=state.web3.account);
          Js.Promise.all2((
            (Event.description(event)
             |> BsWeb3.Eth.call_with(transaction_data)),
            (Event.myTickets(event)
             |> BsWeb3.Eth.call_with(transaction_data))))
          |> Js.Promise.then_ (((description,tickets)) => 
              self.send(MyEventData({event,description,tickets,address})) 
              |> Js.Promise.resolve);
          ()
        })
      }

    | MyEventData(data) => { Js.Array.push(data,state.myEvents); ReasonReact.Update({...state,myEvents:state.myEvents}) }

    | BuyEventAddress(address) => 
        let event = Event.ofAddress(state.web3.web3,address);
        ReasonReact.UpdateWithSideEffects({...state,event_address:address,buy_data:Some({event,numTickets:0,totalCost:BsWeb3.Utils.toBN(0)})},(self) => 
          switch(state.buy_data) {
          | None => 
            self.send(NumTickets(1))
          | Some({numTickets}) => 
            self.send(NumTickets(numTickets))
          }
        )
    | NumTickets(numTickets) => 
        Js.log(numTickets);
        Js.log(state);
        assert(state.buy_data != None);
        let buy_data = Js.Option.getExn(state.buy_data);
        ReasonReact.UpdateWithSideEffects({...state,buy_data:Some({...buy_data,numTickets})},(self) => {
          Event.getCostFor(Js.Option.getExn(state.buy_data).event,~numTickets)
          |> BsWeb3.Eth.call 
          |> Js.Promise.then_ ((totalCost) => { Js.log(totalCost); self.send(TotalCost(totalCost)) |> Js.Promise.resolve});
          ()
        })
    | TotalCost(totalCost) => 
        Js.log(state);
        assert(state.buy_data != None);
        let buy_data = Js.Option.getExn(state.buy_data);
        ReasonReact.Update({...state,buy_data:Some({...buy_data,totalCost})})
    | SubmitBuy => ReasonReact.UpdateWithSideEffects(state,(self) => {
        Js.log(state);
        assert(state.buy_data != None);
        let {event,numTickets,totalCost} = Js.Option.getExn(state.buy_data);
        Event.buy(event,~numTickets=numTickets)
        |> BsWeb3.Eth.send(
            BsWeb3.Eth.make_transaction_with_value(
              ~value=totalCost,~from=state.web3.account))
        |> Js.Promise.then_ (_ => Js.Promise.resolve());
        ()
      })
    }
  },
  render: ({send,state}) =>
<div className="row">

  <div className="col-lg">
    <div className="card container-card">
      <h5 className="card-header">(text("Buy Tickets"))</h5>
      <div className="card-body padding-vertical-less"> 
        <div className="form-group" style=(ReactDOMRe.Style.make(~margin="3%",()))>
          <div className="row">
            <label className="col col-5 col-form-label text-muted">(text("Event"))</label>
            <input className="col form-control" type_="text" placeholder="" id="inputLarge"
                   onChange=(event => send(BuyEventAddress(ReactEvent.Form.target(event)##value)))
                   value=state.event_address
            />
          </div>
          (switch(state.buy_data) {
           | None => ReasonReact.null 
           | Some({numTickets,totalCost}) => [|
              <div key="NumTickets" className="row">
                <label className="col col-5 col-form-label text-muted">(text("Number of tickets"))</label>
                <input className="col form-control" type_="text" placeholder="" id="inputLarge"
                       onChange=(event => send(NumTickets(int_of_float(Js.Float.fromString(ReactEvent.Form.target(event)##value)))))
                       value=(string_of_int(numTickets))
                />
              </div>,
              <div key="TotalCost" className="row">
                <label className="col col-5 col-form-label text-muted">(text("Total Cost"))</label>
                <label className="col col-5 col-form-label">
                  <WeiLabel amount=totalCost/>
                </label>
              </div>,
              <div key="SubmitBuy" className="row">
                <button className="btn btn-success" onClick=(_ => send(SubmitBuy))
                        style=(ReactDOMRe.Style.make(~marginLeft="20px",~marginRight="20px",~marginTop="20px",~width="100%",())) >
                  (text("Submit"))
                </button>
              </div>
            |] |> ReasonReact.array
            }
          )
        </div>
      </div>
    </div>
  </div>

  <div className="col-lg">
    <div className="card container-card">
      <h5 className="card-header">(text("Tickets"))</h5> 
      <div className="card-body">

        <table className="table table-hover border-secondary border-solid">
          <thead className="bg-secondary">
            <tr>
              <th scope="col">(text("Description"))</th>
              <th scope="col">(text("Address"))</th>
              <th scope="col">(text("Tickets"))</th>
            </tr>
          </thead>
          <tbody>
            (state.myEvents |> Js.Array.map(({event,description,address,tickets}) => {
              <tr key=address 
                  /* Support toggle */
                  >
                <td>(text(description))</td>
                <td><AddressLabel address=address uri=state.web3.address_uri /></td>
                <td>(int(Js.Array.length(tickets)))</td>
              </tr>
            })
          |> ReasonReact.array)
          </tbody>
        </table> 

      </div>
    </div>
  </div>


</div>

};

