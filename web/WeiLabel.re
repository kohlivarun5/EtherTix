let component = ReasonReact.statelessComponent("WeiLabel");

let make = (~amount,_children) => {
  /* spread the other default fields of component here and override a few */
  ...component,
  render: self => {
    <div>(string_of_int(amount)
          |> ReasonReact.string)
    </div>
  },
};

