let component = ReasonReact.statelessComponent("AddressLabel");

let make = (~address,_children) => {
  /* spread the other default fields of component here and override a few */
  ...component,
  render: self => {
    <div>(address
          |> Js.String.substring(~from=0,~to_=10)
          |> ReasonReact.string)
    </div>
  },
};
