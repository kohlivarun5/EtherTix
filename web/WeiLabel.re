let component = ReasonReact.statelessComponent("WeiLabel");

let make = (~amount,_children) => {
  /* spread the other default fields of component here and override a few */
  ...component,
  render: self => {
    <div> (
      BsWeb3.Utils.fromWei(amount,"ether") 
      |> BsWeb3.Types.toString(10)
      |> Js.String.concat(" ETH")
      |> ReasonReact.string
    )
    </div>
  },
};

