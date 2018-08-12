/* This is the basic component. */

module Network = Rinkeby;

type web3_state = {
  web3 : BsWeb3.Web3.t,
  address : BsWeb3.Eth.address         
}

type state = {
  web3 : option(web3_state),
  description : string,
};

type action =
  | Submit
  | Change(string)
  | InitWeb3(web3_state)
 
let text = ReasonReact.string;

let component = ReasonReact.reducerComponent("Page");

let make = (_children) => {
  ...component,
  didMount: self => { 

    let w3 = BsWeb3.Web3.makeWeb3(BsWeb3.Web3.currentProvider);
    let eth = BsWeb3.Web3.eth(w3);
    Js.Promise.then_((accounts) => {
      self.send(InitWeb3({
          web3:w3,
          address:accounts[0]
      }));
      Js.Promise.resolve(());
    }) (BsWeb3.Eth.getAccounts(eth));
    ()
  },
  initialState: () => {
    description : "",
    web3:None
  },
  reducer: action => {
    switch (action) {
    | Submit => (state => {
        Js.log(state);

        /* Don't change code untill universe creation 
           As Bs does not support send with new 
           */
        let {web3,address} = Js.Option.getExn(state.web3);
        let eth = BsWeb3.Web3.eth(web3);
        Js.log(eth);
        Js.log(Universe.abi);
        Js.log(Rinkeby.universe);
        let universe:Universe.t = [%bs.raw{| new eth.Contract(UniverseAbiJson.default,RinkebyAddressesJson.default.universe) |}];

        BsWeb3.Eth.send(Universe.createEvent(universe,state.description),BsWeb3.Eth.make_transaction(~from=address))
        |> Js.Promise.then_ ((value) => Js.log(value) -> Js.Promise.resolve);

        ReasonReact.NoUpdate 
      })
    | Change(text) => (state => ReasonReact.Update({...state,description: text}))
    | InitWeb3(web3_state) => (state => ReasonReact.Update({...state,web3:Some(web3_state)}))
    }
  },
  render: ({send,state}) =>
<div>
  <nav className="navbar navbar-expand-lg navbar-dark bg-success">
    <a className="navbar-brand" href="#">(ReasonReact.string("SmartTix"))</a>
  </nav>

  <p/>

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
