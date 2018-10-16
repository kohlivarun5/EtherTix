let text = ReasonReact.string;
let int(i) = i |> string_of_int |> ReasonReact.string;

type buy_data = {
  event:Event.t,
  numTickets:int,
  totalCost:BsWeb3.Types.big_number,
  numSold:int,
  numUnSold:int
};

type ticket_sig = 
| Used 
| UnUsed(string)

type ticket_id_sig = (ticket_sig,int)
type event_data = {
  address:BsWeb3.Eth.address,
  event:Event.t,
  description : string,
  tickets:Js.Array.t(int),
  ticket_signatures:Js.Array.t(ticket_id_sig)
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
| SignTickets(int)
| TicketSignatures(int,Js.Array.t(ticket_id_sig))
| NumSoldUnsold(int,int)

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
          Event.description(event)
          |> BsWeb3.Eth.call_with(transaction_data)
          |> Js.Promise.then_ ((description) => {
              Event.myTickets(event)
              |> BsWeb3.Eth.call_with(transaction_data)
              |> Js.Promise.then_ ((tickets) => {
                  Js.log(description);
                  Js.log(tickets);
                  self.send(MyEventData({event,description,tickets,address,ticket_signatures:[||]}))
                  |> Js.Promise.resolve
              })
          });
          ()
        })
      }

    | MyEventData(data) => { Js.Array.push(data,state.myEvents); ReasonReact.Update({...state,myEvents:state.myEvents}) }

    | BuyEventAddress(event_address) => 
        let event = Event.ofAddress(state.web3.web3,event_address);
        ReasonReact.UpdateWithSideEffects({
          ...state,
          event_address,buy_data:Some({
            event,numTickets:0,
            totalCost:BsWeb3.Utils.toBN(0),
            numSold:0, numUnSold:0 
        })},(self) => {
          switch(state.buy_data) {
          | None => 
            self.send(NumTickets(1))
          | Some({numTickets}) => 
            self.send(NumTickets(numTickets))
          };
          let tx = BsWeb3.Eth.make_transaction(~from=state.web3.account);
          Event.numSold(event)
          |> BsWeb3.Eth.call_with(tx)
          |> Js.Promise.then_ ((numSold) => {
              Event.numUnSold(event)
              |> BsWeb3.Eth.call_with(tx)
              |> Js.Promise.then_ ((numUnSold) => {
                  self.send(NumSoldUnsold(numSold,numUnSold))
                  |> Js.Promise.resolve 
              })
          });
          ()
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
    | NumSoldUnsold(numSold,numUnSold) => 
        Js.log(state);
        assert(state.buy_data != None);
        let buy_data = Js.Option.getExn(state.buy_data);
        ReasonReact.Update({...state,buy_data:Some({...buy_data,numSold,numUnSold})})
    | SubmitBuy => ReasonReact.UpdateWithSideEffects(state,(self) => {
        Js.log(state);
        assert(state.buy_data != None);
        let {event,numTickets,totalCost} = Js.Option.getExn(state.buy_data);
        Js.log(state.buy_data);
        Event.buy(event,~numTickets=numTickets)
        |> BsWeb3.Eth.send(
            BsWeb3.Eth.make_transaction_with_value(
              ~value=totalCost,~from=state.web3.account))
        |> Js.Promise.then_ (_ => Js.Promise.resolve());
        ()
      })
    | SignTickets(index) => ReasonReact.UpdateWithSideEffects(state,(self) => {
        let event = state.myEvents[index].event;
        state.myEvents[index].tickets
        |> Js.Array.reduce((sigs,id) => {

            sigs
            |> Js.Promise.then_ ((sigs) => {
                Js.log("Asking for sign");

                Event.ticketUsed(event,id)
                |> BsWeb3.Eth.call 
                |> Js.Promise.then_((isUsed) => {
                    if (isUsed) {
                      Js.Promise.resolve(Array.append([|(Used,id)|],sigs))
                    } else {
                      Event.ticketVerificationCode(event,id)
                      |> BsWeb3.Eth.call 
                      |> Js.Promise.then_((code) =>
                        state.web3.web3 
                        |> BsWeb3.Web3.eth 
                        |> BsWeb3.Eth.sign(code,state.web3.account))
                      |> Js.Promise.then_((signature) => {
                          Js.log(signature);
                          Event.isOwnerSig(event,id,signature)
                          |> BsWeb3.Eth.call 
                          |> Js.Promise.then_((isOwner) => {
                              assert(isOwner);
                              Js.Promise.resolve(Array.append([|(UnUsed(signature),id)|],sigs))
                            })
                      })
                      |> Js.Promise.catch((_) => {
                          Js.Promise.resolve(Array.append([|(Used,id)|],sigs))
                        })
                    }
                });
            })

            /* Cannot use eth_signTypedData on Toshi
            let msg_params = [|
              BsWeb3.Web3.msg_params(
                ~name="Event Address",
                ~type__="address",
                ~value=event_address),
              BsWeb3.Web3.msg_params(
                ~name="Event Description",
                ~type__="string",
                ~value=state.myEvents[index].description),
              BsWeb3.Web3.msg_params(
                ~name="Ticket Id",
                ~type__="uint256",
                ~value=id)
            |];
            let send_params = 
              BsWeb3.Web3.send_async_params(
                ~method__="eth_signTypedData",
                ~params=(msg_params,state.web3.account),
                ~from=state.web3.account);

            Js.Promise.make((~resolve,~reject) => {
              state.web3.web3 
              |> BsWeb3.Web3.currentProvider__fromWeb3 
              |> BsWeb3.Web3.sendAsync(send_params,((error,result) => 
                  resolve(.
                    (id,BsWeb3.Web3.async_result_sha(result))
                  )
              ))
            })
            */
        },Js.Promise.resolve([||]))
        |> Js.Promise.then_((sigs) => { 
            let sorted_sigs = 
              sigs 
              |> Js.Array.sortInPlaceWith(((sig1,id1),(sig2,id2)) => {
                  if (sig1 == Used && sig2 != Used) { 1 }
                  else if (sig1 != Used && sig2 == Used) { -1 }
                  else { 0 }
                });
            self.send(TicketSignatures(index,sorted_sigs))
            |> Js.Promise.resolve 
          });
        ()
      });
    | TicketSignatures(index,ticket_signatures) => {
        Js.log(ticket_signatures);
        let event = state.myEvents[index];
        let event = {...event,ticket_signatures};
        state.myEvents[index] = event;
        ReasonReact.Update(state)
      }
    }
  },
  render: ({send,state}) =>
<div className="row">

  <div className="col-md">
    <div className="card container-card">
      <h5 className="card-header card-title">(text("Buy Tickets"))</h5>
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
           | Some({numTickets,totalCost,numSold,numUnSold}) => [|
              <div key="Sold" className="row">
                <label className="col col-form-label text-muted">(text("Sold"))</label>
                <label className="col col-form-label"> (int(numSold)) </label>
                <label className="col col-form-label text-muted">(text("Not Sold"))</label>
                <label className="col col-form-label"> (int(numUnSold)) </label>
              </div>,
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
                <button className="btn btn-success btn-send" onClick=(_ => send(SubmitBuy))
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

  <div className="col-md">
    <div className="card container-card">
      <h5 className="card-header card-title">(text("My Tickets"))</h5> 
      <table className="table table-hover border-secondary border-solid table-no-bottom">
        <thead className="bg-secondary">
          <tr>
            <th scope="col">(text("Description"))</th>
            <th scope="col">(text("Address"))</th>
            <th scope="col">(text("Tickets"))</th>
          </tr>
        </thead>
        <tbody>
          (state.myEvents |> Js.Array.mapi((({event,description,address,tickets,ticket_signatures}),i) => {
            [|
            <tr key=(Js.String.concat(string_of_int(i),address ))
                onClick=(_ => send(SignTickets(i)))
                >
              <td>(text(description))</td>
              <td><AddressLabel address=address uri=state.web3.address_uri /></td>
              <td>(int(Js.Array.length(tickets)))</td>
            </tr>,
            (Js.Array.length(ticket_signatures) <= 0 
              ? ReasonReact.null
              : <tr key=(Js.String.concat("Qr",Js.String.concat(string_of_int(i),address)))>
                  <td colSpan=3 style=(ReactDOMRe.Style.make(~maxWidth="170px",()))> 
                    <Carousel> 
                      (ticket_signatures |> Js.Array.map( ((signature,id)) => {
                        switch(signature) {
                          | Used => 
                            <img src="Ticket.svg" 
                              key=string_of_int(id)
                              style=(ReactDOMRe.Style.make(~marginTop="15px",~height="228px",()))
                            />
                          | UnUsed(sha) => {
                            <QrView 
                              style=(ReactDOMRe.Style.make(~marginTop="15px",~marginBottom="10px",()))
                              key=string_of_int(id) text=Js.String.concatMany([|"|",string_of_int(id)|],sha )
                            />
                          }
                        }
                      }
                      ))
                    </Carousel>
                  </td>
                </tr> 
            )
          |] |> ReasonReact.array
          })
          |> ReasonReact.array)
        </tbody>
      </table> 
    </div>
  </div>


</div>

};

