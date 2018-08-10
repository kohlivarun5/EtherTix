/* This is the basic component. */
let component = ReasonReact.statelessComponent("Page");

let text = ReasonReact.string;

/* Your familiar handleClick from ReactJS. This mandatorily takes the payload,
   then the `self` record, which contains state (none here), `handle`, `reduce`
   and other utilities */
let onChange (_event, _self) = Js.log(_event);
let onSubmit (_event, _self) = Js.log(_event);

/* `make` is the function that mandatorily takes `children` (if you want to use
   `JSX). `message` is a named argument, which simulates ReactJS props. Usage:

   `<Page message="hello" />`

   Which desugars to

   `ReasonReact.element(Page.make(~message="hello", [||]))` */
let make = (~message, _children) => {
  ...component,
  render: self =>
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
               onInput=(event => Js.log(event))
        />
      </div>
    </div>
  </div>
  <div className="card-footer">
    <button type_="submit" className="col btn btn-success" 
            onClick=(event => Js.log(event)) >
      (text("Submit"))
    </button>
  </div>

</div>

</div>

};
