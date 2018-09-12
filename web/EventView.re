let text = ReasonReact.string;
let int(i) = i |> string_of_int |> ReasonReact.string;

type sold_data = {
  numSold:int,
  numUnsold:int
}

type issue_data = {
  number:int,
  price_milli:int
}

type state = {
  web3 : Web3.state,
  address : BsWeb3.Eth.address,
  event: Event.t,
  data:sold_data,
  issue_data:issue_data,
  show_scanner:bool
};
type action = 
| FetchData
| SoldData(sold_data) 
| IssueNumber(int) 
| IssuePrice(int)
| SubmitIssue
| Withdraw 
| ToggleScanner

let component = ReasonReact.reducerComponent("EventView");

let make = (~web3,~address,~event,_children) => {
  ...component,
  initialState: () => { 
    web3:web3,
    address:address,
    event:event,
    data:{numSold:0,numUnsold:0},
    issue_data:{number:100,price_milli:100},
    show_scanner:false
  },
  didMount: ({send}) => send(FetchData),
  reducer: (action,state:state) => {
    switch (action) {
    | FetchData => ReasonReact.UpdateWithSideEffects(state,(self) => {
        let tx = BsWeb3.Eth.make_transaction(~from=state.web3.account);
        Event.numSold(state.event)
        |> BsWeb3.Eth.call_with(tx)
        |> Js.Promise.then_ ((numSold) => {
            Event.numUnSold(state.event)
            |> BsWeb3.Eth.call_with(tx)
            |> Js.Promise.then_ ((numUnsold) => {
                self.send(SoldData({numSold:numSold,numUnsold:numUnsold}))
                |> Js.Promise.resolve 
            })
        });
        ()
      })

    | SoldData(data) => ReasonReact.Update({...state,data:data})
    | IssueNumber(number) => ReasonReact.Update({...state,issue_data:{...state.issue_data,number}})
    | IssuePrice(price_milli) => ReasonReact.Update({...state,issue_data:{...state.issue_data,price_milli}})
    | SubmitIssue => ReasonReact.UpdateWithSideEffects(state,(self) => {
        Event.issue(state.event,
                    ~number=state.issue_data.number,
                    ~price=(
                      BsWeb3.Utils.toWei(
                        BsWeb3.Utils.toBN(state.issue_data.price_milli),
                        "milliether")
                    ))
        |> BsWeb3.Eth.send(BsWeb3.Eth.make_transaction(~from=state.web3.account))
        |> Js.Promise.then_ (_ => self.send(FetchData) |> Js.Promise.resolve);
        ()
      })
    | Withdraw => ReasonReact.UpdateWithSideEffects(state,(self) => {
        Event.withdraw(state.event)
        |> BsWeb3.Eth.send(BsWeb3.Eth.make_transaction(~from=state.web3.account))
        |> Js.Promise.then_ (_ => self.send(FetchData) |> Js.Promise.resolve);
        ()
      })
    | ToggleScanner => ReasonReact.Update({...state,show_scanner:!state.show_scanner})
    }
  },
  render: ({send,state}) =>
<div className="card">
  

  <div className="card-header">
    <button className="btn btn-success btn-send" onClick=(_ => send(Withdraw))
            style=(ReactDOMRe.Style.make(~width="100%",())) 
      >

      (text("Withdraw"))
    </button>
  </div>
    
  <div className="card-body"> 
    <div className="row">
      <div className="col col-5">
        <div className="row">
          <div className="col text-muted">(text("Sold"))</div>
          <div className="col">(int(state.data.numSold))</div>
        </div>
      </div>
      <div className="col col-7">
        <div className="row">
          <div className="col text-muted">(text("Not Sold"))</div>
          <div className="col">(int(state.data.numUnsold))</div>
        </div>
      </div>
    </div>

    (!state.show_scanner 
     ? ReasonReact.null
     : <QrReader 
        style=(ReactDOMRe.Style.make(~marginTop="20px",()))
        onScan=((result) => Js.log(result))
        onError=((error) => Js.log(error)) />
    )

    <div className="padding-vertical-less">
      <button className="btn btn-success btn-send" onClick=(_ => send(ToggleScanner))
              style=(ReactDOMRe.Style.make(~marginTop="20px",~width="100%",())) 
        >
        (text(state.show_scanner ? "Hide Scanner" : "Scan Tickets"))
      </button>
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
        <label className="col col-5 col-form-label text-muted">(text("Price per ticket (milli ETH)"))</label>
        <input className="col form-control" type_="text" placeholder="" id="inputLarge" 
               onChange=(event => send(IssuePrice(ReactEvent.Form.target(event)##value)))
               value=(string_of_int(state.issue_data.price_milli))
        />
      </div>
      <div className="row">
        <button className="btn btn-success btn-send" onClick=(_ => send(SubmitIssue))
                style=(ReactDOMRe.Style.make(~marginLeft="20px",~marginRight="20px",~marginTop="20px",~width="100%",())) >
          (text("Submit"))
        </button>
      </div>
    </div>
  </div>

</div>
};
