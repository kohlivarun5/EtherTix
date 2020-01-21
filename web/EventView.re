let text = ReasonReact.string;
let int(i) = i |> string_of_int |> ReasonReact.string;

external string_of_any : 'a => string = "%identity"

type sold_data = {
  numSold:int,
  numUnsold:int
}

type used_data = {
  numUsed:int,
  numToBeUsed:int
}

type issue_data = {
  number:int,
  price_milli:int
}

type state = {
  web3 : Web3State.t,
  address : BsWeb3.Eth.address,
  description:string,
  imgSrc:string,
  event: Event.t,
  sold_data:sold_data,
  used_data:used_data,
  issue_data:issue_data,
};
type action = 
| FetchData
| SoldData(sold_data) 
| UsedData(used_data) 
| IssueNumber(int) 
| IssuePrice(int)
| SubmitIssue
| Withdraw 
| ScanTickets
| ClearCache 
| UseTicket(string)
| Img(string)
| SetImg

let component = ReasonReact.reducerComponent("EventView");

let make = (~web3,~description, ~imgSrc, ~address,~event,_children) => {
  ...component,
  initialState: () => { 
    web3:web3,
    address:address,
    description:description,
    imgSrc:imgSrc,
    event:event,
    sold_data:{numSold:0,numUnsold:0},
    used_data:{numUsed:0,numToBeUsed:0},
    issue_data:{number:100,price_milli:100},
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
                self.send(SoldData({numSold,numUnsold}))
                |> Js.Promise.resolve 
            })
        });
        Event.numUsed(state.event)
        |> BsWeb3.Eth.call_with(tx)
        |> Js.Promise.then_ ((numUsed) => {
            Event.numToBeUsed(state.event)
            |> BsWeb3.Eth.call_with(tx)
            |> Js.Promise.then_ ((numToBeUsed) => {
                self.send(UsedData({numUsed,numToBeUsed}))
                |> Js.Promise.resolve 
            })
        });
        ()
      })
    | SoldData(sold_data) => ReasonReact.Update({...state,sold_data})
    | UsedData(used_data) => ReasonReact.Update({...state,used_data})
    | IssueNumber(number) => ReasonReact.Update({...state,issue_data:{...state.issue_data,number}})
    | Img(imgSrc) => ReasonReact.Update({...state,imgSrc})
    | IssuePrice(price_milli) => ReasonReact.Update({...state,issue_data:{...state.issue_data,price_milli}})
    | SetImg => ReasonReact.UpdateWithSideEffects(state,(_) => {
        Event.setImg(state.event, state.imgSrc)
        |> BsWeb3.Eth.send(BsWeb3.Eth.make_transaction(~from=state.web3.account))
        |> Js.Promise.then_ (Js.Promise.resolve);
        ()
      })

    | SubmitIssue => ReasonReact.UpdateWithSideEffects(state,(self) => {
        Js.log("SubmitIssue");
        Js.log(state.issue_data.number);
        Js.log(state.issue_data.price_milli);
        Js.log(BsWeb3.Utils.toWei(
                        BsWeb3.Utils.toBN(state.issue_data.price_milli),
                        "milliether")
                    );
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
    | ClearCache => ReasonReact.UpdateWithSideEffects(state,(_) => {
        Dom.Storage.clear(Dom.Storage.localStorage);
        BsUtils.alert("Tickets cache cleared!");
      })
    | ScanTickets => ReasonReact.UpdateWithSideEffects(state,(self) => {
        BsWeb3.Web3.scanQRCode(state.web3.web3,[%bs.re "/.*/"])
        |> Js.Promise.then_((code) => {
            self.send(UseTicket(code)) |> Js.Promise.resolve
        })
        /*
        |> Js.Promise.catch((error) => {
            BsUtils.alert(
              "Invalid code:"
              |> Js.String.concat(string_of_any(error)))
            |> Js.Promise.resolve
        })
        */
        |> ignore

      })
    | UseTicket(code) => ReasonReact.UpdateWithSideEffects(state,(self) => {
        Js.log(code);
        Js.log(Dom.Storage.localStorage);
        let (signature,token) = 
          switch (Js.String.split("|",code)) {
            | [|signature,token|] => (signature,token)
            | _ => BsUtils.alert(
                    "Ticket code invalid:\n"
                    |> Js.String.concat(code));
                   assert(false)
          };
        Js.log(signature);
        Js.log(token);
        Event.ticketUsed(state.event,int_of_string(token))
        |> BsWeb3.Eth.call 
        |> Js.Promise.then_ ((res) => {
            if (res || Dom.Storage.getItem(code,Dom.Storage.localStorage) != None) { 
              BsUtils.alert("Ticket already used!");
              self.send(ScanTickets)
              |> Js.Promise.resolve
            } else {
              Event.isOwnerSig(state.event,int_of_string(token),signature)
              |> BsWeb3.Eth.call 
              |> Js.Promise.then_ ((res) => {
                if (!res) {
                  BsUtils.alert("Invalid ticket code");
                  self.send(ScanTickets)
                  |> Js.Promise.resolve
                } else {
                  Dom.Storage.setItem(code,code,Dom.Storage.localStorage);
                  Event.useTicket(state.event,int_of_string(token),signature)
                  |> BsWeb3.Eth.send(BsWeb3.Eth.make_transaction(~from=state.web3.account))
                  |> Js.Promise.then_ ((_) => { 
                      self.send(FetchData) 
                      |> Js.Promise.resolve 
                  });
                  self.send(ScanTickets)
                  |> Js.Promise.resolve
                }
              });
            }
          });
        ()
      })
    }
  },
  render: ({send,state}) =>
<div className="card">
  

  <div className="card-header padding-horizontal-less">
    (switch(state.imgSrc) {
      | "" => ReasonReact.null
      | imgSrc => <img className="event-img" src=imgSrc />
    })

    <div className="form-group" style=(ReactDOMRe.Style.make(~margin="3%",()))>
      <div className="row">
        <label className="col col-4 col-form-label text-muted" > (text("Image")) </label>
        <input className="col form-control" type_="text" placeholder="" id="inputLarge"
               onChange=(event => send(Img(ReactEvent.Form.target(event)##value)))
               value=state.imgSrc
               style=(ReactDOMRe.Style.make(~marginRight="10px",()))
        />
        <button className="col col-3 btn btn-success btn-send" onClick=(_ => send(SetImg))
                style=(ReactDOMRe.Style.make(~marginRight="20px",~width="100%",())) >
          (text("Save"))
        </button>

      </div>
    </div>
  </div>

  <div className="card-body row" >
    <div className="col text-center font-weight-bold" 
         style=ReactDOMRe.Style.make(~alignSelf="center",()) >(text("Share Event"))
    </div>
    <div className="col text-center" 
         style=ReactDOMRe.Style.make(~alignSelf="center",())>
      <a href=BsUtils.createSearchUri("event",address)>
        <img className="share-icon" src="img/Sharethis.png" />
      </a>
    </div>
    <div className="col text-center" 
         style=ReactDOMRe.Style.make(~alignSelf="center",())>
      <a href=(
            "sms: &body="
            |> Js.String.concat(
                BsUtils.createEventLinkUriComponent(~address=state.address,~description=state.description))
            )>
        <img className="share-icon" src="img/imessage_logo.png" />
      </a>
    </div>
    <div className="col text-center" 
         style=ReactDOMRe.Style.make(~alignSelf="center",())>
      <a href=(
            "whatsapp://send?text="
            |> Js.String.concat(
                BsUtils.createEventLinkUriComponent(~address=state.address,~description=state.description))
            )>
        <img className="share-icon" src="img/whatsapp_logo2.png" />
      </a>
    </div>
  </div>
  
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
          <div className="col col-5">(int(state.sold_data.numSold))</div>
        </div>
      </div>
      <div className="col col-7 padding-horizontal-less">
        <div className="row">
          <div className="col text-muted padding-horizontal-less">(text("Available"))</div>
          <div className="col col-5">(int(state.sold_data.numUnsold))</div>
        </div>
      </div>
    </div>

    <div className="row">
      <div className="col col-5">
        <div className="row">
          <div className="col text-muted">(text("Used"))</div>
          <div className="col col-5">(int(state.used_data.numUsed))</div>
        </div>
      </div>
      <div className="col col-7 padding-horizontal-less">
        <div className="row">
          <div className="col text-muted padding-horizontal-less">(text("To Be Used"))</div>
          <div className="col col-5">(int(state.used_data.numToBeUsed))</div>
        </div>
      </div>
    </div>


    <div className="padding-vertical-less">
      (switch(state.web3.web3 
              |> BsWeb3.Web3.getScanQrCode
              |> Js.Undefined.toOption) {
        | None => (
          <button disabled=true className="btn btn-warning"
                  style=(ReactDOMRe.Style.make(~marginTop="20px",~width="100%",())) >
            (text("Scanner not available"))
          </button>
        )
        | Some(_) => (
          <div>
            <button className="btn btn-success btn-send" 
                    onClick=(_ => send(ScanTickets))
                    style=(ReactDOMRe.Style.make(~marginTop="20px",~width="100%",()))>
              (text("Scan Tickets"))
            </button>
            <button className="btn btn-warning" 
                    onClick=(_ => send(ClearCache))
                    style=(ReactDOMRe.Style.make(~marginTop="20px",~width="100%",()))>
              (text("Clear Cache"))
            </button>
          </div>
        )
      })
    </div>
  </div>

  <h6 className="card-header">(text("Issue Tickets"))</h6>
  <div className="card-body padding-vertical-less padding-horizontal-less" > 
    <div className="form-group" style=(ReactDOMRe.Style.make(~margin="3%",()))>
      <div className="row row-margin">
        <label className="col col-8 col-form-label text-muted" > (text("Number of tickets")) </label>
        <input className="col form-control" type_="text" placeholder="" id="inputLarge"
               onChange=(event => send(IssueNumber(ReactEvent.Form.target(event)##value)))
               value=(string_of_int(state.issue_data.number))
               style=(ReactDOMRe.Style.make(~marginRight="10px",()))
        />
      </div>
      <div className="row row-margin">
        <label className="col col-8 col-form-label text-muted" > (text("Ticket price (milli ETH)")) </label>
        <input className="col form-control" type_="text" placeholder="" id="inputLarge" 
               onChange=(event => send(IssuePrice(ReactEvent.Form.target(event)##value)))
               value=(string_of_int(state.issue_data.price_milli))
               style=(ReactDOMRe.Style.make(~marginRight="10px",()))
        />
      </div>
      <div className="row">
        <button className="btn btn-success btn-send" onClick=(_ => send(SubmitIssue))
                style=(ReactDOMRe.Style.make(~marginLeft="20px",~marginRight="20px",~marginTop="10px",~width="100%",())) >
          (text("Submit"))
        </button>
      </div>
    </div>
  </div>

</div>
};
