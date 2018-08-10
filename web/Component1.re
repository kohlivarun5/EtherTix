/* This is the basic component. */

type state = {
  description : string,
};

type action =
  | Submit
  | Change(string)

let text = ReasonReact.string;

let component = ReasonReact.reducerComponent("Page");

let make = (~message, _children) => {
  ...component,
  initialState: () => {
    description : "",
  },
  reducer: action => {
    switch (action) {
    | Submit => (state => { Js.log(state); ReasonReact.NoUpdate } )
    | Change(text) => (
        state => ReasonReact.Update({...state, description: text})
      )
    }
  },
  render: ({state, handle, send}) =>
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
            onClick=(e => send(Submit))>
      (text("Submit"))
    </button>
  </div>

</div>

</div>

};
