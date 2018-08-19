let component = ReasonReact.statelessComponent("WeiLabel");

let make = (~amount,_children) => {
  /* spread the other default fields of component here and override a few */
  ...component,
  render: self => {
    <div> (
      switch(amount) {
        | 0 => "0" 
        | _ => string_of_float(float_of_int(amount) /. 1000000000000000000.)
      }
      |> Js.String.concat(" ETH")
      |> ReasonReact.string
    )
    </div>
  },
};

