/* This is the basic component. */

module Network = Rinkeby;

type web3_state = {
  web3 : BsWeb3.Web3.t,
  address : BsWeb3.Eth.address,
  universe: Universe.t,
  
}



type state = {
  web3 : option(web3_state),
  new_event_description : string,
  myEvents : Js.Array.t(Event.data)
};

type action =
  | Submit
  | Change(string)
  | InitWeb3(web3_state)
  | EventData(Event.data)
 
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

        let {address,universe} = Js.Option.getExn(state.web3);
        BsWeb3.Eth.send(Universe.createEvent(universe,state.new_event_description),BsWeb3.Eth.make_transaction(~from=address))
        |> Js.Promise.then_ ((value) => Js.log(value) -> Js.Promise.resolve);

        ReasonReact.NoUpdate 
      })
    | Change(text) => (state => ReasonReact.Update({...state,new_event_description: text}))
    | EventData(data) => (state => { Js.Array.push(data,state.myEvents); ReasonReact.Update({...state,myEvents:state.myEvents}) })
    | InitWeb3(web3_state) => (state => {
        Js.log("Returning with side effects");
        ReasonReact.UpdateWithSideEffects({...state,web3:Some(web3_state)}, (self) => {

          Js.log("Calling myEvents");
          BsWeb3.Eth.call_with(Universe.myEvents(web3_state.universe),BsWeb3.Eth.make_transaction(~from=web3_state.address))
          |> Js.Promise.then_ ((events_addr:Js.Array.t(BsWeb3.Eth.address)) => {
    
              Js.log("Got Events");
              Js.log(events_addr);
              events_addr |> Js.Array.map((addr) => {
                let eth = BsWeb3.Web3.eth(web3_state.web3);
                  
                Js.log(eth);
                Js.log(Event.abi);
                let event:Event.t = [%bs.raw{| new eth.Contract(EventAbiJson.default,addr) |}];

                Event.description(event)
                |> BsWeb3.Eth.call 
                |> Js.Promise.then_ ((des) => self.send(EventData({Event.description:des,address:addr})) |> Js.Promise.resolve)
   
              })
              |> Js.Promise.resolve;
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
  <h3 className="card-header">(text("My Events"))</h3>
  <div className="card-body">

    <table className="table table-hover">
      <thead>
        <tr>
          <th scope="col">(text("Description"))</th>
          <th scope="col">(text("Address"))</th>
        </tr>
      </thead>
      (state.myEvents |> Js.Array.map(({Event.description,address}) => {
        <tbody>
          <tr className="table-active">
            <th scope="row">(text(description))</th>
            <td>(text(address))</td>
          </tr>
        </tbody>
      })
      |> ReasonReact.array)
    </table> 

  </div>
</div>

<div className="card" style=(ReactDOMRe.Style.make(~margin="10%",()))>
  <h3 className="card-header">(text("Create Event"))</h3>
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
