/* This is the basic component. */

type event_data = {
  address:BsWeb3.Eth.address,
  event:Event.t,
  description : string,
  balance : BsWeb3.Types.big_number,
  show:bool
};

type state = {
  web3 : option(Web3.state),
  new_event_description : string,
  myEvents : Js.Array.t(event_data)
};

type action =
  | Submit
  | Change(string)
  | GetOrganizerEvents(Web3.state)
  | AddEvent(BsWeb3.Eth.address)
  | EventData(event_data)
  | ToggleEvent(BsWeb3.Eth.address)
 
let text = ReasonReact.string;

let component = ReasonReact.reducerComponent("OrganizerView");

let make = (_children) => {
  ...component,
  didMount: self => { 
    if (Js.typeof(BsWeb3.Web3.get) !== "undefined") {
      let w3_global = Js.Undefined.getExn(BsWeb3.Web3.get);
      let w3 = BsWeb3.Web3.makeWeb3(BsWeb3.Web3.currentProvider(w3_global));
      let eth = BsWeb3.Web3.eth(w3);
      BsWeb3.Eth.net(eth)
      |> BsWeb3.Net.getId
      |> Js.Promise.then_ ((networkId) => {
          let {NetworkInfo.universe,address_uri} = (
            switch(networkId) {
            | 1 => MainNet.t
            | 3 => Ropsten.t
            | 4 => Rinkeby.t
            | _ => Js.log(networkId);assert(false);
            }
          );

          BsWeb3.Eth.getAccounts(eth)
          |> Js.Promise.then_((accounts) => {

            /* Don't change code untill universe creation 
               As Bs does not support send with new 
               */
            let universe_address = Js.String.concat("",universe);
            Js.log(eth);
            Js.log(universe_address);
            Js.log(Universe.abi);
            let universe:Universe.t = [%bs.raw{| new eth.Contract(UniverseAbiJson.default,universe_address) |}];

            Js.log("GetOrganizerEvents");
            self.send(GetOrganizerEvents({
                web3:w3,
                account:accounts[0],
                universe,
                address_uri 
            }))
            |> Js.Promise.resolve
          })
      });
      ()
    } else {
      Js.log("Web3 is undefined!");
    }
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
        ReasonReact.UpdateWithSideEffects(state, (_) => {
          let {Web3.account,universe} = Js.Option.getExn(state.web3);
          Universe.createEvent(universe,state.new_event_description)
          |> BsWeb3.Eth.send(BsWeb3.Eth.make_transaction(~from=account));
          ()
        })
      })
    | ToggleEvent(address) => (state => {
        let events = 
          state.myEvents 
          |> Js.Array.map((event) => {
              ...event,
              show:((event.address === address) ? !event.show : false)
            });
        let state = {...state,myEvents:events};
        ReasonReact.Update(state)
      })
    | Change(text) => (state => ReasonReact.Update({...state,new_event_description: text}))
    | EventData(data) => (state => { 
      let index = Js.Array.findIndex((({address}) => address == data.address),state.myEvents);
      if (index > -1) {
        state.myEvents[index] = data;
      } else {
        Js.Array.push(data,state.myEvents) |> ignore
      };
      ReasonReact.Update({...state,myEvents:state.myEvents})
    })
    | AddEvent(address) => (state => {
        ReasonReact.UpdateWithSideEffects(state,(self) => {
          let web3_state = Js.Option.getExn(state.web3);
          let event = Event.ofAddress(web3_state.web3,address);
          let transaction_data = BsWeb3.Eth.make_transaction(~from=web3_state.account);

          Event.getBalance(event)
          |> BsWeb3.Eth.call_with(transaction_data)
          |> Js.Promise.then_ ((balance) => {
              Event.description(event)
              |> BsWeb3.Eth.call_with(transaction_data)
              |> Js.Promise.then_ ((description) => {
                  Js.log(description);
                  Js.log(balance);
                  self.send(EventData({event,description,balance,address,show:false}))
                  |> Js.Promise.resolve
              })
          });
          ()
        })
      })
    | GetOrganizerEvents(web3_state) => (state => {
        Js.log("Returning with side effects");
        ReasonReact.UpdateWithSideEffects({...state,web3:Some(web3_state)}, (self) => {
          Js.log("Calling myEvents");
          Universe.organizerEvents(
            web3_state.universe,
            Universe.filter_options(
              ~filter=Universe.organizerEventsQuery(~organizerAddr=web3_state.account,~active=true,()),
              ~fromBlock=0,()),
            ((error,eventData) => {
              Js.log(error);
              self.send(AddEvent(Universe.organizerEventAddr(eventData)))
            })
          );
          ()
        });
      })
    }
  },
  render: ({send,state}) =>
<div>
  <nav className="navbar navbar-expand-lg navbar-dark bg-success fixed-top">
    <a className="navbar-brand" href="#">(ReasonReact.string("BlockTix"))</a>
  </nav>

  <div className="container">

    (state.web3 === None 
      ? ReasonReact.null 
      : <UserView web3=Js.Option.getExn(state.web3) />
    )

    <div className="row">

      <div className="col-md">
        <div className="card container-card">
          <h5 className="card-header card-title">(text("Create Event"))</h5>
          <div className="card-body padding-vertical-less">

            <div className="form-group" style=(ReactDOMRe.Style.make(~margin="3%",()))>
              <div className="row">
                <label className="col col-5 col-form-label text-muted">(text("Description"))</label>
                <input className="col form-control" type_="text" placeholder="" id="inputLarge" 
                       value=state.new_event_description
                       onChange=(event => send(Change(ReactEvent.Form.target(event)##value)))
                />
              </div>
            </div>
          </div>
          <div className="card-footer">
            <button disabled={state.web3 == None} type_="submit" className="col btn btn-success btn-send" 
                    onClick=(_ => send(Submit))>
              (text("Submit"))
            </button>
          </div>

        </div>
      
      </div>

      <div className="col-md">
        <div className="card container-card">
          <h5 className="card-header card-title">(text("Organized Events"))</h5> 
          <table className="table table-hover border-secondary border-solid table-no-bottom">
            <thead className="bg-secondary">
              <tr>
                <th scope="col">(text("Description"))</th>
                <th scope="col">(text("Address"))</th>
                <th scope="col">(text("Balance"))</th>
              </tr>
            </thead>
            <tbody>
              (state.myEvents |> Js.Array.map(({event,description,address,balance,show}) => {
                [|
                <tr key=address 
                    className=((show) ? "table-active bg-black" : "")
                    onClick=(_ => send(ToggleEvent(address)))>
                  <td>(text(description))</td>
                  <td><AddressLabel address=address uri=Js.Option.getExn(state.web3).address_uri /></td>
                  <td><WeiLabel amount=balance/></td>
                </tr>,
                (switch (show) {
                 | true => 
                    <tr className="table-active bg-black" key=(Js.String.concat(address,"View"))>
                      <td colSpan=3>
                        <EventView description=description event=event address=address web3=Js.Option.getExn(state.web3) />
                      </td>
                    </tr>
                 | false => ReasonReact.null 
                 })
                |] |> ReasonReact.array
              })
            |> ReasonReact.array)
            </tbody>
          </table> 
        </div>
      </div>
      
    </div>

    (state.web3 === None 
      ? ReasonReact.null 
      : <AdminView web3=Js.Option.getExn(state.web3) />
    )

  
  </div>

</div>

};
