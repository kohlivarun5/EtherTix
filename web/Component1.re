/* This is the basic component. */

module Network = Rinkeby;

type web3_state = {
  web3 : BsWeb3.Web3.t,
  address : BsWeb3.Eth.address,
  universe: Universe.t,
  
}

type data = {
  address:BsWeb3.Eth.address,
  description : string,
  balance : int
};

type state = {
  web3 : option(web3_state),
  new_event_description : string,
  myEvents : Js.Array.t(data)
};

type action =
  | Submit
  | Change(string)
  | InitWeb3(web3_state)
  | AddEvent(BsWeb3.Eth.address)
  | EventData(data)
 
let text = ReasonReact.string;

let component = ReasonReact.reducerComponent("Page");

let make = (_children) => {
  ...component,
  didMount: self => { 

    let w3 = BsWeb3.Web3.makeWeb3(BsWeb3.Web3.currentProvider);
    let eth = BsWeb3.Web3.eth(w3);
    Js.Promise.then_((accounts) => {

      /* Don't change code untill universe creation 
         As Bs does not support send with new 
         */
      Js.log(eth);
      Js.log(Universe.abi);
      Js.log(Rinkeby.universe);
      let universe:Universe.t = [%bs.raw{| new eth.Contract(UniverseAbiJson.default,RinkebyAddressesJson.default.universe) |}];

      Js.log("InitWeb3");
      self.send(InitWeb3({
          web3:w3,
          address:accounts[0],
          universe:universe
      }));
      Js.Promise.resolve(());
    }) (BsWeb3.Eth.getAccounts(eth));
    ()
  },
  initialState: () => {
    new_event_description : "",
    web3:None,
    myEvents:[||]
  },
  reducer: action => {
    switch (action) {
    | Submit => (state => {
        Js.log(state);
        ReasonReact.UpdateWithSideEffects(state, (self) => {
          let {address,universe} = Js.Option.getExn(state.web3);
          Universe.createEvent(universe,state.new_event_description)
          |> BsWeb3.Eth.send(BsWeb3.Eth.make_transaction(~from=address))
          |> Js.Promise.then_ ((addr) => self.send(AddEvent(addr)) |> Js.Promise.resolve);
          ()
        })
      })
    | Change(text) => (state => ReasonReact.Update({...state,new_event_description: text}))
    | EventData(data) => (state => { Js.Array.push(data,state.myEvents); ReasonReact.Update({...state,myEvents:state.myEvents}) })
    | AddEvent(address) => (state => {
        ReasonReact.UpdateWithSideEffects(state,(self) => {
          let web3_state = Js.Option.getExn(state.web3);
          let eth = BsWeb3.Web3.eth(web3_state.web3);
            
          Js.log(eth);
          Js.log(Event.abi);
          let event:Event.t = [%bs.raw{| new eth.Contract(EventAbiJson.default,address) |}];

          let transaction_data = BsWeb3.Eth.make_transaction(~from=web3_state.address);
          Js.Promise.all2((
            (Event.description(event)
             |> BsWeb3.Eth.call_with(transaction_data)),
            (Event.getBalance(event)
             |> BsWeb3.Eth.call_with(transaction_data))))
          |> Js.Promise.then_ (((description,balance)) => 
              self.send(EventData({description:description,balance:balance,address:address})) 
              |> Js.Promise.resolve);
          ()
        })
      })
    | InitWeb3(web3_state) => (state => {
        Js.log("Returning with side effects");
        ReasonReact.UpdateWithSideEffects({...state,web3:Some(web3_state)}, (self) => {

          Js.log("Calling myEvents");
          let transaction_data = BsWeb3.Eth.make_transaction(~from=web3_state.address);
          Universe.myEvents(web3_state.universe)
          |> BsWeb3.Eth.call_with(transaction_data)
          |> Js.Promise.then_ ((events_addr:Js.Array.t(BsWeb3.Eth.address)) => {
              Js.log("Got Events");
              Js.log(events_addr);
              events_addr |> Js.Array.map((addr) => { self.send(AddEvent(addr)) });
              Js.Promise.resolve();
          });
          ()
        });
      })
    }
  },
  render: ({send,state}) =>
<div>
  <nav className="navbar navbar-expand-lg navbar-dark bg-success">
    <a className="navbar-brand" href="#">(ReasonReact.string("SmartTix"))</a>
  </nav>

  <p/>

<div className="card" style=(ReactDOMRe.Style.make(~margin="10%",()))>
  <h4 className="card-header">(text("My Events"))</h4> 
  <div className="card-body">

    <table className="table table-hover">
      <thead>
        <tr>
          <th scope="col">(text("Description"))</th>
          <th scope="col">(text("Address"))</th>
          <th scope="col">(text("Balance"))</th>
        </tr>
      </thead>
      (state.myEvents |> Js.Array.map(({description,address,balance}) => {
        <tbody>
          <tr className="table-active">
            <th scope="row">(text(description))</th>
            <td><AddressLabel address=address/></td>
            <td><WeiLabel amount=balance/></td>
          </tr>
        </tbody>
      })
      |> ReasonReact.array)
    </table> 

  </div>
</div>

<div className="card" style=(ReactDOMRe.Style.make(~margin="10%",()))>
  <h4 className="card-header">(text("Create Event"))</h4>
  <div className="card-body">

    <div className="form-group" style=(ReactDOMRe.Style.make(~margin="3%",()))>
      <div className="row">
        <label className="col col-5 col-form-label text-muted">(text("Description"))</label>
        <input className="col form-control" type_="text" placeholder="" id="inputLarge" 
               onInput=(event => send(Change(ReactEvent.Form.target(event)##value)))
        />
      </div>
    </div>
  </div>
  <div className="card-footer">
    <button disabled={state.web3 == None} type_="submit" className="col btn btn-success" 
            onClick=(_ => send(Submit))>
      (text("Submit"))
    </button>
  </div>

</div>

</div>

};
