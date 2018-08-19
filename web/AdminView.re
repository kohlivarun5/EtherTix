let text = ReasonReact.string;

type state = {
  web3 : Web3.state,
  universe_balance : BsWeb3.Types.big_number
};

type action = 
| Init 
| Balance(BsWeb3.Types.big_number)
| Withdraw

let component = ReasonReact.reducerComponent("AdminView");

let make = (~web3,_children) => {
  ...component,
  initialState: () => { 
    web3:web3,
    universe_balance:BsWeb3.Utils.toBN(0)
  },
  didMount: self => { self.send(Init) },
  reducer: (action,state:state) => {
    switch (action) {
    | Init => {
        ReasonReact.UpdateWithSideEffects(state, (self) => {
          let transaction_data = BsWeb3.Eth.make_transaction(~from=state.web3.account);
          Universe.getBalance(state.web3.universe)
          |> BsWeb3.Eth.call_with(transaction_data)
          |> Js.Promise.then_ ( (balance) => {
              self.send(Balance(balance)) |> Js.Promise.resolve
            });
          ()
        })
      }
    | Balance(universe_balance) => { ReasonReact.Update({...state,universe_balance}) }

    | Withdraw => ReasonReact.UpdateWithSideEffects(state,(self) => {
        Universe.withdraw(state.web3.universe)
        |> BsWeb3.Eth.send(BsWeb3.Eth.make_transaction(~from=state.web3.account))
        |> Js.Promise.then_ (_ => self.send(Init) |> Js.Promise.resolve);
        ()
      })

    }
  },
  render: ({send,state}) =>
<div>

  <div className="card container-card">
    <h5 className="card-header">(text("Admin Information"))</h5>
    <div className="card-body padding-vertical-less"> 
      <div className="form-group" style=(ReactDOMRe.Style.make(~margin="3%",()))>
        <div className="row">
          <label className="col col-5 col-form-label text-muted">(text("Balance"))</label>
          <label className="col col-5 col-form-label">
            <WeiLabel amount=state.universe_balance />
          </label>
        </div>
      </div>
    </div>
    <div className="card-header">
      <button className="btn btn-success" onClick=(_ => send(Withdraw))
              style=(ReactDOMRe.Style.make(~width="100%",())) 
        >

        (text("Withdraw"))
      </button>
    </div>
  </div>

</div>

};


