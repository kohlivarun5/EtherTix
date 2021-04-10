let text = ReasonReact.string;
let int(i) = i |> string_of_int |> ReasonReact.string;

let eventOfAddress(web3,address) {
  let eth = BsWeb3.Web3.eth(web3);
  Js.log((eth,address,Event.abi));
  let event:Event.t = [%bs.raw{| new eth.Contract(EventAbiJson.default,address) |}];
  event 
};

type buy_data = {
  event:Event.t,
  description:string,
  imgSrc:string,
  numTickets:int,
  totalCost:BsWeb3.Types.big_number,
  numSold:int,
  numUnSold:int,
  resale_tickets:Js.Array.t((int,BsWeb3.Types.big_number))
};

type ticket_sig = 
| Used 
| UnUsed(string)

type ticket_id_sig = (ticket_sig,int)
type ticket_info = {
  token:int,
  price:BsWeb3.Types.big_number,
  for_sale:bool,
  used:bool
}

type my_event_data = {
  address:BsWeb3.Eth.address,
  event:Event.t,
  description : string,
  imgSrc: string,
  tickets:Js.Array.t(ticket_info),
  show_details:bool,
  ticket_signatures:Js.Array.t(ticket_id_sig)
}

type event_data = {
  address:BsWeb3.Eth.address,
  description : string,
}

type state = {
  web3 : Web3State.t,
  event_address:BsWeb3.Eth.address,
  buy_data:option(buy_data),
  myEvents:Js.Array.t(my_event_data),
  allEvents:Js.Array.t(event_data),
  selling_price_per_ticket:BsWeb3.Types.big_number
};

type action = 
| GetMyEvents 
| GetAllEvents
| AddEvent(BsWeb3.Eth.address)
| MyEventData(my_event_data)
| EventData(event_data)
| BuyEventAddress(BsWeb3.Eth.address)
| BuyEventDescription(string,string)
| NumTickets(int)
| TotalCost(BsWeb3.Types.big_number)
| SubmitBuy 
| ToggleDetails(int)
| SignTickets(int)
| TicketSignatures(int,Js.Array.t(ticket_id_sig))
| NumSoldUnsoldResale(int,int,Js.Array.t((int,BsWeb3.Types.big_number)))
| SellingPricePerTicket(int,int,BsWeb3.Types.big_number)
| ProposeSale(int,int)
| RetractSale(int,int)
| BuyResale(Event.t,int,BsWeb3.Types.big_number)

let component = ReasonReact.reducerComponent("UserView");

let make = (~web3,_children) => {
  ...component,
  initialState: () => { 
    web3:web3,
    event_address:"",
    buy_data:None,
    myEvents:[||],
    allEvents:[||],
    selling_price_per_ticket:BsWeb3.Utils.toBN(0)
  },
  didMount: self => { 
    self.send(GetMyEvents);
    self.send(GetAllEvents);
    switch(BsUtils.getSearchValueByKey("event")) {
      | None => ()
      | Some(address) => self.send(BuyEventAddress(address))
    }
  },
  reducer: (action,state:state) => {
    switch (action) {
    | GetMyEvents => {
        Js.log("GetMyEvents");
        ReasonReact.UpdateWithSideEffects(state, (self) => {
          Universe.userEvents(
            state.web3.universe.contract,
            Universe.filter_options(
              ~filter=Universe.userEventsQuery(~userAddr=state.web3.account,()),
              ~fromBlock=0,~toBlock="latest",()))
          |> Js.Promise.then_((events) => {
            Js.log(events);
            events |> Js.Array.map((event) => 
              self.send(AddEvent(Universe.userEventAddr(event)))
            ) |> ignore |> Js.Promise.resolve
          });
          ()
        })
      }
    | GetAllEvents => {
        ReasonReact.UpdateWithSideEffects(state, (self) => {
          Universe.organizerEvents(
            state.web3.universe.contract,
            Universe.filter_options(~fromBlock=0,~toBlock="latest",()))
          |> Js.Promise.then_((events) => 
            events |> Js.Array.map((event) => 
              self.send(
                EventData({
                  address:Universe.organizerEventAddr(event),
                  description:Universe.organizerEventDesc(event)
                })
              )
            ) |> ignore |> Js.Promise.resolve 
          );
          ()
        })
      }
    | AddEvent(address) => {
        ReasonReact.UpdateWithSideEffects(state,(self) => {
          let event = eventOfAddress(state.web3.web3,address);
          Event.ERC721MetaData.name(event)
          |> BsWeb3.Eth.call
          |> Js.Promise.then_ ((description) => {
              Event.Info.imgSrc(event)
              |> BsWeb3.Eth.call
              |> Js.Promise.then_ ((imgSrc) => {
                Event.ERC721.balanceOf(event,state.web3.account)
                |> BsWeb3.Eth.call
                |> Js.Promise.then_ ((tokens) => {
                    Js.log(tokens);
                    [%bs.raw {| new Array(parseInt(tokens)).fill(0) |}]
                    |> Js.Array.mapi((_,index) => {
                        Js.log(("Index=",index));
                        Event.ERC721Enumerable.tokenOfOwnerByIndex(event,state.web3.account,index)
                        |> BsWeb3.Eth.call
                        |> Js.Promise.then_ ((token) => {
                          Js.log((index,token));

                          Event.Info.ticketInfo(event,token)
                          |> BsWeb3.Eth.call
                          |> Js.Promise.then_ (((used,prev_price,owner,for_sale,ask)) => {
                            Js.log((used,prev_price,owner,for_sale,ask));
                            { token,used, price:BsWeb3.Utils.fromWei(for_sale ? ask : prev_price,"milliether"), for_sale }
                            |> Js.Promise.resolve
                          })
                        })
                    })
                    |> Js.Promise.all
                    |> Js.Promise.then_ ((tickets) => {
                      Js.log(tickets);
                      self.send(MyEventData({
                        tickets,event,imgSrc,description,address,show_details:false,ticket_signatures:[||]
                      }))
                      |> Js.Promise.resolve
                    })
                })
              })
          });
          ()
        })
      }
    | MyEventData(data) => { 
      let index = Js.Array.findIndex(((({address}:my_event_data)) => address == data.address),state.myEvents);
      if (index > -1 ) {
        state.myEvents[index] = data;
      } else {
        Js.Array.push(data,state.myEvents) |> ignore
      };
      ReasonReact.Update({...state,myEvents:state.myEvents})
    }

    | EventData(data) => { 
      let index = Js.Array.findIndex(((({address}:event_data)) => address == data.address),state.allEvents);
      if (index > -1 ) {
        state.allEvents[index] = data;
      } else {
        Js.Array.push(data,state.allEvents) |> ignore
      };
      ReasonReact.Update({...state,allEvents:state.allEvents})
    }

    | BuyEventAddress(event_address) => 
        let event = eventOfAddress(state.web3.web3,event_address);
        ReasonReact.UpdateWithSideEffects({
          ...state,
          event_address,buy_data:Some({
            event,numTickets:0,imgSrc:"",description:"",
            totalCost:BsWeb3.Utils.toBN(0),
            numSold:0, numUnSold:0,
            resale_tickets:[||]
        })},(self) => {
          switch(state.buy_data) {
          | None => 
            self.send(NumTickets(1))
          | Some({numTickets}) => 
            self.send(NumTickets(numTickets))
          };
          Event.ERC721MetaData.name(event)
          |> BsWeb3.Eth.call 
          |> Js.Promise.then_ ((description) => 
              Event.Info.imgSrc(event)
              |> BsWeb3.Eth.call
              |> Js.Promise.then_ ((imgSrc) =>
                self.send(BuyEventDescription(description,imgSrc))
                |> Js.Promise.resolve)
              );
          Event.ERC721Enumerable.totalSupply(event)
          |> BsWeb3.Eth.call
          |> Js.Promise.then_ ((numTotal) => {

            Event.Info.numSoldUsed(event)
            |> BsWeb3.Eth.call
            |> Js.Promise.then_ (((numSold,_)) => {
                let numUnSold = numTotal - numSold;

                Event.Market.askEvents(event, Event.Market.filter_options(~fromBlock=0,~toBlock="latest"))
                |> Js.Promise.then_ ((ask_events) => {
                    ask_events
                    |> Js.Array.map(Event.Market.ask_event_token)
                    |> Js.Array.map ((token) => {
                        Event.Info.ticketInfo(event,token)
                        |> BsWeb3.Eth.call
                        |> Js.Promise.then_ (((used,_,_,for_sale,ask)) => {
                          (token,used,for_sale,ask)
                          |> Js.Promise.resolve
                        })
                    })
                    |> Js.Promise.all
                    |> Js.Promise.then_ ((tickets) => {
                      let resale_tickets = 
                        tickets
                        |> Js.Array.filter (((_,used,for_sale,_)) => !used && for_sale)
                        |> Js.Array.map(((token,_,_,ask)) => (token,ask));
                        Js.log((numSold,numUnSold,resale_tickets));
                        self.send(NumSoldUnsoldResale(numSold,numUnSold,resale_tickets))
                        |> Js.Promise.resolve 
                    })
                })
              })
          });
          ()
        }
        )
    | BuyEventDescription(description,imgSrc) =>
        assert(state.buy_data != None);
        let buy_data = Js.Option.getExn(state.buy_data);
        ReasonReact.Update({...state,buy_data:Some({...buy_data,description,imgSrc})})
    | NumTickets(numTickets) => 
        Js.log(numTickets);
        Js.log(state);
        assert(state.buy_data != None);
        let buy_data = Js.Option.getExn(state.buy_data);
        ReasonReact.UpdateWithSideEffects({...state,buy_data:Some({...buy_data,numTickets})},(self) => {
          Event.Info.getCostFor(Js.Option.getExn(state.buy_data).event,numTickets)
          |> BsWeb3.Eth.call 
          |> Js.Promise.then_ ((totalCost) => { 
              Js.log(totalCost); 
              let totalCost = 
                (BsWeb3.Types.toString(10,totalCost)
                |> Js.String.length) > 75 ? BsWeb3.Utils.toBN(0) : totalCost;
              self.send(TotalCost(totalCost)) 
              |> Js.Promise.resolve
            });
          ()
        })
    | TotalCost(totalCost) => 
        Js.log(state);
        assert(state.buy_data != None);
        let buy_data = Js.Option.getExn(state.buy_data);
        ReasonReact.Update({...state,buy_data:Some({...buy_data,totalCost})})
    | NumSoldUnsoldResale(numSold,numUnSold,resale_tickets) => 
        Js.log(state);
        assert(state.buy_data != None);
        let buy_data = Js.Option.getExn(state.buy_data);
        ReasonReact.Update({...state,buy_data:Some({...buy_data,numSold,numUnSold,resale_tickets})})
    | SubmitBuy => ReasonReact.UpdateWithSideEffects(state,(self) => {
        Js.log(state);
        assert(state.buy_data != None);
        let {event,numTickets,totalCost} = Js.Option.getExn(state.buy_data);
        Js.log(state.buy_data);
        Event.Edit.buy(event,numTickets)
        |> BsWeb3.Eth.send(
            BsWeb3.Eth.make_transaction_with_value(
              ~value=totalCost,~from=state.web3.account))
        |> Js.Promise.then_ (_ => self.send(GetMyEvents) |> Js.Promise.resolve);
        ()
      })
    | BuyResale(event,token,price) => ReasonReact.UpdateWithSideEffects(state,(self) => {
        Event.Market.hitAsk(event,token)
        |> BsWeb3.Eth.send(
            BsWeb3.Eth.make_transaction_with_value(
              ~value=price,~from=state.web3.account))
        |> Js.Promise.then_ (_ => self.send(BuyEventAddress(state.event_address)) |> Js.Promise.resolve);
        ()
      })
    | SignTickets(index) => ReasonReact.UpdateWithSideEffects(state,(self) => {
        let event = state.myEvents[index].event;
        state.myEvents[index].tickets
        |> Js.Array.reduce((sigs,{token}) => {

            sigs
            |> Js.Promise.then_ ((sigs) => {
                Js.log("Asking for sign");

                Event.Info.ticketInfo(event,token)
                |> BsWeb3.Eth.call 
                |> Js.Promise.then_(((isUsed,_,_,_,_)) => {
                    if (isUsed) {
                      Js.Promise.resolve(Array.append([|(Used,token)|],sigs))
                    } else {
                      Event.Use.verificationCode(event,328)
                      |> BsWeb3.Eth.call 
                      |> Js.Promise.then_((code) =>
                        state.web3.web3 
                        |> BsWeb3.Web3.eth 
                        |> BsWeb3.Eth.sign(code,state.web3.account))
                      |> Js.Promise.then_((signature) => {
                          Js.log(signature);
                          Event.Use.isOwnerSig(event,token,signature)
                          |> BsWeb3.Eth.call 
                          |> Js.Promise.then_((isOwner) => {
                              assert(isOwner);
                              Js.Promise.resolve(Array.append([|(UnUsed(signature),token)|],sigs))
                            })
                      })
                      |> Js.Promise.catch((e) => {
                          Js.log(e);
                          Js.Promise.resolve(Array.append([|(Used,token)|],sigs));
                        })
                    }
                });
            })

            /* Cannot use eth_signTypedData on Toshi
            let msg_params = [|
              BsWeb3.Web3.msg_params(
                ~name="Event ID",
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
              |> BsWeb3.Web3.getCurrentProvider
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
              |> Js.Array.sortInPlaceWith(((sig1,_),(sig2,_)) => {
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
    | ToggleDetails(index) => {
        let event = state.myEvents[index];
        let event = {...event,show_details:(Js.Array.length(event.tickets) <= 0 ? false : !event.show_details)};
        state.myEvents[index] = event;
        ReasonReact.Update(state)
      }
    | SellingPricePerTicket(event_index,ticket_index,price) => 
        let {tickets} = state.myEvents[event_index];
        let ticket = tickets[ticket_index];
        tickets[ticket_index] = {...ticket,price};
        ReasonReact.Update(state);
    | ProposeSale(index,ticket_index) => ReasonReact.UpdateWithSideEffects(state,(_) => {
        let {event,tickets} = state.myEvents[index];
        let {token,price}= tickets[ticket_index];
        Event.Market.proposeSales(event,[|token|],[|BsWeb3.Utils.toWeiBN(price,"milliether")|])
        |> BsWeb3.Eth.send(
            BsWeb3.Eth.make_transaction(~from=state.web3.account))
        |> Js.Promise.then_ (_ => Js.Promise.resolve());
        ()
      })
    | RetractSale(index,ticket_index) => ReasonReact.UpdateWithSideEffects(state,(_) => {
        let {event,tickets} = state.myEvents[index];
        let {token} = tickets[ticket_index];
        Event.Market.retractSales(event,[|token|])
        |> BsWeb3.Eth.send(
            BsWeb3.Eth.make_transaction(~from=state.web3.account))
        |> Js.Promise.then_ (_ => Js.Promise.resolve());
        ()
      })
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
            <label className="col col-6 col-form-label text-muted">(text("Event ID"))</label>
            <input className="col form-control" type_="text" placeholder="" id="inputLarge"
                   onChange=(event => send(BuyEventAddress(ReactEvent.Form.target(event)##value)))
                   value=state.event_address
            />
          </div>
          </div>
      </div>
      (switch(state.buy_data) {
       | None => ReasonReact.null 
       | Some({event,imgSrc,description,numTickets,totalCost,numSold,numUnSold,resale_tickets}) => <div>
          <h6 className="card-header bg-success text-light font-weight-bold">(text(description))</h6>
          (switch(imgSrc) { | "" => ReasonReact.null | imgSrc => <div className="card-header"><img className="event-img" src=imgSrc /></div> })
          <div key="Sold" className="card-body row padding-vertical-less">
            <label className="col col-3 col-form-label text-muted">(text("Sold"))</label>
            <label className="col col-3 col-form-label padding-horizontal-less"> (int(numSold)) </label>
            <label className="col col-3 col-form-label text-muted padding-horizontal-less">(text("Available"))</label>
            <label className="col col-3 col-form-label padding-horizontal-less"> (int(numUnSold)) </label>
          </div>
          <div key="NumTickets" className="row card-body padding-vertical-less">
            <label className="col col-6 col-form-label text-muted">(text("Number of tickets"))</label>
            <input autoFocus=true className="col form-control" type_="text" placeholder="" id="inputLarge"
                   style=(ReactDOMRe.Style.make(~marginRight="10px",()))
                   onChange=(event => send(NumTickets(int_of_float(Js.Float.fromString(ReactEvent.Form.target(event)##value)))))
                   value=(string_of_int(numTickets))
            />
          </div>
          <div key="TotalCost" className="row card-body padding-vertical-less">
            <label className="col col-form-label text-muted">(text("Total Cost"))</label>
            <label className="col col-6 col-form-label padding-horizontal-less">
              <WeiLabel amount=totalCost/>
            </label>
          </div>
          <div key="SubmitBuy" className="row card-body padding-vertical-less">
            <button className=Js.String.concat(" btn btn-send ",(
                                numTickets == 0 
                                ? "btn-secondary" 
                                : (BsWeb3.Types.toString(10,totalCost) == "0" 
                                    ? "btn-danger"
                                    : "btn-success")))
                    onClick=(_ => send(SubmitBuy))
                    style=(ReactDOMRe.Style.make(~marginLeft="20px",~marginRight="20px",~marginBottom="10px",~width="100%",())) 
                    disabled={BsWeb3.Types.toString(10,totalCost) == "0" }>
              (text(BsWeb3.Types.toString(10,totalCost) == "0" && numTickets != 0 
                    ? "Sold Out!"
                    : "Buy Now"))
            </button>
          </div>
          (Js.Array.length(resale_tickets) <= 0 ? ReasonReact.null : <div>
            <div className="card-header font-weight-bold padding-vertical-less">(text("Resale Tickets"))</div> 
            <div className="card-body"
                 style=(ReactDOMRe.Style.make(~paddingTop="5px",~paddingBottom="10px",()))
                >
              (resale_tickets |> Js.Array.map(((i,price)) => {
                <div key=string_of_int(i) className="row"
                     style=(ReactDOMRe.Style.make(~marginTop="10px",()))>
                  <label className="col col col-form-label text-muted padding-horizontal-less">(text("Ticket Price"))</label>
                  <label className="col col-4 col-form-label padding-horizontal-less"
                         style=(ReactDOMRe.Style.make(~textAlign="center",()))> 
                    <WeiLabel amount=price/> 
                  </label>
                  <button className="col col-4 col-form-button btn btn-success btn-send" 
                          onClick=(_ => send(BuyResale(event,i,price)))
                          style=(ReactDOMRe.Style.make(
                                  ~marginRight="10px",
                                  ~width="100%",())) >
                    (text("Buy Now"))
                  </button>
                </div>
              }) |> ReasonReact.array )
            </div>
          </div>)
        </div> 
      })
      (state.buy_data != None || Js.Array.length(state.allEvents) <= 0 ? ReasonReact.null : 
      <div>
        <div className="card-header font-weight-bold padding-vertical-less"
            style=ReactDOMRe.Style.make(~borderBottom="0px",())>(text("Events Listing"))</div> 
        <table className="table table-hover border-secondary border-solid table-no-bottom tableBodyScroll">
          <tbody>
            (state.allEvents |> Js.Array.mapi((({description,address}),i) => {
              <tr key=(Js.String.concat(string_of_int(i),address )) 
                  onClick=((_) => send(BuyEventAddress(address)))
                >
                <td>(text(description))</td>
                <td><AddressLabel address=address uri=state.web3.address_uri /></td>
              </tr> 
            }) |> ReasonReact.array)
          </tbody>
        </table>
      </div>)
    </div>
  </div>
  
  (Js.Array.length(state.myEvents) <= 0 ? ReasonReact.null : 
  <div className="col-md">
    <div className="card container-card">
      <h5 className="card-header card-title bg-danger">(text("My Tickets"))</h5> 
      <table className="table table-hover border-secondary border-solid table-no-bottom">
        <thead className="bg-secondary">
          <tr>
            <th scope="col">(text("Description"))</th>
            <th scope="col">(text("ID"))</th>
            <th scope="col">(text("Tickets"))</th>
          </tr>
        </thead>
        <tbody>
          (state.myEvents |> Js.Array.mapi((({description,address,tickets,show_details,ticket_signatures}),i) => {
            [|
            <tr key=(Js.String.concat(string_of_int(i),address ))
                onClick=(_ => send(ToggleDetails(i)))
                className=((show_details) ? "table-active bg-black" : "")
                >
              <td>(text(description))</td>
              <td><AddressLabel address=address uri=state.web3.address_uri /></td>
              <td>(int(Js.Array.length(tickets)))</td>
            </tr>,
            (show_details 
             ?  <tr key=(Js.String.concat(string_of_int(i),"details"))
                    className=((show_details) ? "table-active bg-black" : "")>
                  <td colSpan=3 style=(ReactDOMRe.Style.make(~maxWidth="170px",())) >
                    <div className="card">
                      ((tickets |> Js.Array.every(({used}) => used))
                       ?(<div className="card-header font-weight-bold padding-vertical-less bg-light">(text("All Tickets Used"))</div>)
                       :<div>
                        (Js.Array.length(ticket_signatures) <= 0 
                         ? <div className="card-body">
                            <button className="btn btn-success btn-send" onClick=(_ => send(SignTickets(i)))
                              style=(ReactDOMRe.Style.make(~width="100%",())) >
                              (text("Get Tickets"))
                            </button>
                           </div>
                         : <div className="card-body padding-vertical-less padding-horizontal-less" > 
                             <Carousel> 
                               (ticket_signatures |> Js.Array.map( ((signature,id)) => {
                                 switch(signature) {
                                   | Used => 
                                     <img src="img/UsedTicket.png"
                                       key=string_of_int(id)
                                       className="used-ticket"
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
                           </div>
                        )
                        <div className="card-header font-weight-bold padding-vertical-less">(text("Resale Tickets"))</div>
                        <div className="card-body padding-vertical-less" style=(ReactDOMRe.Style.make(~paddingLeft="14px",()))> 
                          <div className="form-group" style=(ReactDOMRe.Style.make(~margin="3%",()))>

                            (tickets |> Js.Array.mapi(({price,for_sale,used},token_index) => {
                              <div key=(Js.String.concat("event_tickets",string_of_int(token_index))) className="row">
                                (used 
                                 ? ReasonReact.null
                                 : ([|
                                      <label key=("price_label"++(string_of_int(token_index))) className="col col-6 col-form-label text-muted padding-horizontal-less">(text("Price (milli ETH)"))</label>,
                                      (for_sale 
                                       ? <button key=("retract"++(string_of_int(token_index)))  className="col btn btn-success btn-danger padding-horizontal-less" style=(ReactDOMRe.Style.make(~width="100%",~marginBottom="10px",()))
                                          onClick=(_ => send(RetractSale(i,token_index))) > (text("Retract Sale")) </button>
                                       : ([|
                                            <input key=("price_value"++(string_of_int(token_index))) className="col form-control" type_="text" placeholder=""
                                             onChange=(event => send(SellingPricePerTicket(i,token_index,ReactEvent.Form.target(event)##value)))
                                             value=(BsWeb3.Types.toString(10,price))
                                            />,
                                            <button key=("ProposeSale"++(string_of_int(token_index))) className="col btn btn-success btn-send padding-horizontal-less" style=(ReactDOMRe.Style.make(~width="100%",~marginBottom="10px",~marginLeft="10px",())) 
                                                    onClick=(_ => send(ProposeSale(i,token_index))) > (text("Sell")) </button>
                                          |] |> ReasonReact.array)
                                      )
                                    |] |> ReasonReact.array)
                                )
                              </div>
                            }) |> ReasonReact.array)
                          </div>
                        </div>
                       </div>
                      )
                    </div>
                  </td>
                </tr>
                : ReasonReact.null)
          |] |> ReasonReact.array
          })
          |> ReasonReact.array)
        </tbody>
      </table> 
    </div>
  </div>
  )
  
</div>

};

