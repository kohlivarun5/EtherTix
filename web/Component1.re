/* This is the basic component. */

type state = {
  web3 : option(BsWeb3.Web3.t),
  description : string,
};

type action =
  | Submit
  | Change(string)
  | InitWeb3
 
let text = ReasonReact.string;

let component = ReasonReact.reducerComponent("Page");

let make = (_children) => {
  ...component,
  didMount: self => { self.send(InitWeb3) },
  initialState: () => {
    description : "",
    web3:None 
  },
  reducer: action => {
    switch (action) {
    | Submit => (state => { 
        Js.log(state);
        Js.log(
          switch(state.web3) {
          | Some(web3) => 
            Js.log(web3);
            let eth = BsWeb3.Web3.eth(web3);
            BsWeb3.Eth.getBalance(eth, (BsWeb3.Eth.coinbase(eth)))
          | None => ""
          });
        
        ReasonReact.NoUpdate 
      })
    | Change(text) => (state => ReasonReact.Update({...state,description: text}))
    | InitWeb3 => (state => 
        ReasonReact.Update({
          ...state,
          web3: Some (BsWeb3.Web3.makeWeb3(BsWeb3.Web3.currentProvider))
        })
      )
    }
  },
  render: ({send}) =>
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
    <button type_="submit" className="col btn btn-success" 
            onClick=(_ => send(Submit))>
      (text("Submit"))
    </button>
  </div>

</div>

</div>

};
