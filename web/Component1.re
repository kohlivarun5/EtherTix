/* This is the basic component. */
let component = ReasonReact.statelessComponent("Page");

/* Your familiar handleClick from ReactJS. This mandatorily takes the payload,
   then the `self` record, which contains state (none here), `handle`, `reduce`
   and other utilities */
let handleClick = (_event, _self) => Js.log("clicked!");

/* `make` is the function that mandatorily takes `children` (if you want to use
   `JSX). `message` is a named argument, which simulates ReactJS props. Usage:

   `<Page message="hello" />`

   Which desugars to

   `ReasonReact.element(Page.make(~message="hello", [||]))` */
let make = (~message, _children) => {
  ...component,
  render: self =>
<div>
  <nav className="navbar navbar-expand-lg navbar-dark bg-dark">
    <a className="navbar-brand" href="#">(ReasonReact.string("SmartTix"))</a>
  </nav>

  <p/>

  <div className="jumbotron">
    <h1 className="display-3">(ReasonReact.string("Hello, world!"))</h1>
    <p className="lead">(ReasonReact.string("This is a simple hero unit, a simple jumbotron-style component for calling extra attention to featured content or information."))</p>
    <hr className="my-4"/>
    <p>(ReasonReact.string("It uses utility classes for typography and spacing to space content out within the larger container."))</p>
    <p className="lead">
      <a className="btn btn-primary btn-lg" href="#" role="button">(ReasonReact.string("Learn more"))</a>
    </p>
  </div>

</div>

};
