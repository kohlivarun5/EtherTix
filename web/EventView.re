let text = ReasonReact.string;
let int(i) = i |> string_of_int |> ReasonReact.string;

type sold_data = {
  numSold:int,
  numUnsold:int
}

type issue_data = {
  number:int,
  price_szabo:int
}

type state = {
  web3 : Web3.state,
  address : BsWeb3.Eth.address,
  event: Event.t,
  data:sold_data,
  issue_data:issue_data
};
type action = 
| FetchData
| SoldData(sold_data) 
| IssueNumber(int) 
| IssuePrice(int)
| SubmitIssue

let component = ReasonReact.reducerComponent("EventView");

let make = (~web3,~address,~event,_children) => {
  ...component,
  initialState: () => { 
    web3:web3,
    address:address,
    event:event,
    data:{numSold:0,numUnsold:0},
    issue_data:{number:100,price_szabo:1000}
  },
  didMount: ({send}) => send(FetchData),
  reducer: (action,state:state) => {
    switch (action) {
    | FetchData => ReasonReact.UpdateWithSideEffects(state,(self) => {
        Js.Promise.all2(
          (
            (Event.numSold(state.event)
            |> BsWeb3.Eth.call_with(BsWeb3.Eth.make_transaction(~from=state.web3.account))),
            (Event.numUnSold(state.event)
            |> BsWeb3.Eth.call_with(BsWeb3.Eth.make_transaction(~from=state.web3.account)))
          )
        )
        |> Js.Promise.then_ ( ((numSold,numUnsold)) => 
            self.send(SoldData({numSold:numSold,numUnsold:numUnsold}))
            |> Js.Promise.resolve);
        ()
      })

    | SoldData(data) => ReasonReact.Update({...state,data:data})
    | IssueNumber(number) => ReasonReact.Update({...state,issue_data:{...state.issue_data,number}})
    | IssuePrice(price_szabo) => ReasonReact.Update({...state,issue_data:{...state.issue_data,price_szabo}})
    | SubmitIssue => ReasonReact.UpdateWithSideEffects(state,(self) => {
        Event.issue(state.event,~number=state.issue_data.number,~price_szabo=state.issue_data.price_szabo)
        |> BsWeb3.Eth.send(BsWeb3.Eth.make_transaction(~from=state.web3.account))
        |> Js.Promise.then_ (_ => self.send(FetchData) |> Js.Promise.resolve);
        ()
      })
    }
  },
  render: ({send,state}) =>
<div className="card">
  <div className="card-body"> 
    <div className="row">
      <div className="col">
        <div className="row">
          <div className="col text-muted">(text("Sold : "))</div>
          <div className="col">(int(state.data.numSold))</div>
        </div>
      </div>
      <div className="col">
        <div className="row">
          <div className="col text-muted">(text("UnSold : "))</div>
          <div className="col">(int(state.data.numUnsold))</div>
        </div>
      </div>
    </div>
  </div>

  <h6 className="card-header">(text("Issue Tickets"))</h6>
  <div className="card-body padding-vertical-less"> 
    <div className="form-group" style=(ReactDOMRe.Style.make(~margin="3%",()))>
      <div className="row">
        <label className="col col-5 col-form-label text-muted">(text("Number of tickets"))</label>
        <input className="col form-control" type_="text" placeholder="" id="inputLarge"
               onChange=(event => send(IssueNumber(ReactEvent.Form.target(event)##value)))
               value=(string_of_int(state.issue_data.number))
        />
      </div>
      <div className="row">
        <label className="col col-5 col-form-label text-muted">(text("Price per ticket (szabo)"))</label>
        <input className="col form-control" type_="text" placeholder="" id="inputLarge" 
               onChange=(event => send(IssuePrice(ReactEvent.Form.target(event)##value)))
               value=(string_of_int(state.issue_data.price_szabo))
        />
      </div>
      <div className="row">
        <button className="btn btn-success" onClick=(_ => send(SubmitIssue))
                style=(ReactDOMRe.Style.make(~marginLeft="20px",~marginRight="20px",~marginTop="20px",~width="100%",())) >
          (text("Submit"))
        </button>
      </div>
    </div>
  </div>

</div>
};
